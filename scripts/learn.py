#!/usr/bin/env python3
"""Deterministic Wikipedia learner for parrot0.

One run processes one source row from kb/learning/sources.tsv. It fetches the
Wikipedia page, stores a Markdown copy, extracts a short first-sentence
definition without AI, updates kb/learning/learned.p0, writes an operational
log, and advances a round-robin state file.
"""

from __future__ import annotations

import argparse
import datetime as dt
import hashlib
import html
import json
import re
import sys
import unicodedata
import urllib.parse
import urllib.request
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SOURCES = ROOT / "kb/learning/sources.tsv"
DEFAULT_KB = ROOT / "kb/learning/learned.p0"
DEFAULT_STATE = ROOT / "kb/learning/state.json"
DEFAULT_MANIFEST = ROOT / "kb/learning/index.json"
DEFAULT_PAGES_DIR = ROOT / "kb/learning/pages"
DEFAULT_LOGS_DIR = ROOT / "kb/learning/logs"

USER_AGENT = "parrot0-wikipedia-learner/1.0"
KB_STRING_LIMIT = 120


def die(message: str) -> None:
    print(f"learn.py: {message}", file=sys.stderr)
    raise SystemExit(1)


def utc_now() -> str:
    return (
        dt.datetime.now(dt.timezone.utc)
        .replace(microsecond=0)
        .isoformat()
        .replace("+00:00", "Z")
    )


def ascii_text(value: str) -> str:
    value = html.unescape(value or "")
    value = value.translate(
        {
            ord("\u2018"): "'",
            ord("\u2019"): "'",
            ord("\u201c"): '"',
            ord("\u201d"): '"',
            ord("\u2013"): "--",
            ord("\u2014"): "--",
            ord("\u00a0"): " ",
        }
    )
    value = unicodedata.normalize("NFKD", value)
    value = value.encode("ascii", "ignore").decode("ascii")
    value = re.sub(r"[ \t\r\f\v]+", " ", value)
    return value.strip()


def p0_atom(value: str) -> str:
    value = ascii_text(value).lower().replace("&", " and ")
    value = re.sub(r"['`]", "", value)
    value = re.sub(r"[^a-z0-9]+", "_", value).strip("_")
    if not value:
        value = "concept"
    if value[0].isdigit():
        value = f"c_{value}"
    return value[:96].rstrip("_") or "concept"


def fit_string(value: str, limit: int = KB_STRING_LIMIT) -> str:
    value = ascii_text(value).replace("\\", "/").replace('"', "'")
    value = re.sub(r"\s+", " ", value).strip()
    if len(value) <= limit:
        return value
    cut = value[: max(0, limit - 3)].rstrip()
    space = cut.rfind(" ")
    if space >= limit // 2:
        cut = cut[:space]
    return cut.rstrip(" ,;:.") + "..."


def p0_string(value: str) -> str:
    return f'"{fit_string(value)}"'


def relative(path: Path) -> str:
    try:
        return path.resolve().relative_to(ROOT).as_posix()
    except ValueError:
        return path.as_posix()


def read_sources(path: Path) -> list[dict[str, Any]]:
    if not path.exists():
        die(f"missing sources file: {path}")
    sources: list[dict[str, Any]] = []
    for lineno, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        stripped = raw.strip()
        if not stripped or stripped.startswith("#"):
            continue
        parts = raw.split("\t")
        if len(parts) != 2:
            die(f"{path}:{lineno}: expected '<domain><TAB><Wikipedia title>'")
        domain = p0_atom(parts[0])
        title = ascii_text(parts[1])
        if not domain or not title:
            die(f"{path}:{lineno}: empty domain or title")
        sources.append({"domain": domain, "title": title, "line": lineno})
    if not sources:
        die(f"no sources in {path}")
    return sources


def load_json(path: Path, default: dict[str, Any]) -> dict[str, Any]:
    if not path.exists():
        return default
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        die(f"invalid JSON in {path}: {exc}")


def write_text_if_changed(path: Path, content: str) -> bool:
    path.parent.mkdir(parents=True, exist_ok=True)
    old = path.read_text(encoding="utf-8") if path.exists() else None
    if old == content:
        return False
    path.write_text(content, encoding="utf-8")
    return True


def write_json(path: Path, data: dict[str, Any]) -> bool:
    return write_text_if_changed(
        path, json.dumps(data, indent=2, sort_keys=True, ensure_ascii=True) + "\n"
    )


def http_json(url: str, timeout: int) -> dict[str, Any]:
    request = urllib.request.Request(
        url,
        headers={"Accept": "application/json", "User-Agent": USER_AGENT},
    )
    with urllib.request.urlopen(request, timeout=timeout) as response:
        return json.loads(response.read().decode("utf-8"))


def fetch_summary(title: str, lang: str, timeout: int) -> dict[str, Any]:
    encoded = urllib.parse.quote(title.replace(" ", "_"), safe="")
    data = http_json(
        f"https://{lang}.wikipedia.org/api/rest_v1/page/summary/{encoded}",
        timeout,
    )
    if data.get("type") == "https://mediawiki.org/wiki/HyperSwitch/errors/not_found":
        die(f"Wikipedia page not found: {title}")
    if not data.get("extract"):
        die(f"Wikipedia summary has no extract: {title}")
    return data


def fetch_wikitext(title: str, lang: str, timeout: int) -> tuple[str, dict[str, Any]]:
    params = urllib.parse.urlencode(
        {
            "action": "query",
            "format": "json",
            "formatversion": "2",
            "inprop": "url",
            "prop": "info|revisions",
            "redirects": "1",
            "rvprop": "content|ids|timestamp",
            "rvslots": "main",
            "titles": title,
        }
    )
    data = http_json(f"https://{lang}.wikipedia.org/w/api.php?{params}", timeout)
    pages = data.get("query", {}).get("pages", [])
    if not pages or pages[0].get("missing"):
        die(f"Wikipedia page text not found: {title}")
    page = pages[0]
    revision = (page.get("revisions") or [{}])[0]
    slots = revision.get("slots") or {}
    content = (slots.get("main") or {}).get("content") or revision.get("content") or ""
    return content, page


def load_fixture(path: Path) -> tuple[dict[str, Any], str, dict[str, Any]]:
    data = load_json(path, {})
    summary = data.get("summary")
    if not isinstance(summary, dict):
        die(f"fixture lacks summary object: {path}")
    wikitext = data.get("wikitext", "")
    if not isinstance(wikitext, str):
        die(f"fixture wikitext must be a string: {path}")
    page = data.get("page", {})
    if not isinstance(page, dict):
        die(f"fixture page must be an object: {path}")
    return summary, wikitext, page


def first_sentence(text: str) -> str:
    text = ascii_text(text)
    for match in re.finditer(r"[.!?](?:\s+|$)", text):
        sentence = text[: match.start() + 1].strip()
        if len(sentence) >= 30:
            return sentence
    return text


def title_aliases(title: str) -> list[str]:
    title = re.sub(r"\s*\([^)]*\)", "", ascii_text(title)).strip()
    aliases = [title]
    lower = title.lower()
    if lower.endswith("y"):
        aliases.append(title[:-1] + "ies")
    elif lower.endswith("s"):
        aliases.append(title)
    else:
        aliases.append(title + "s")
    aliases.append(title.replace("-", " "))
    seen: list[str] = []
    for alias in aliases:
        alias = re.sub(r"\s+", " ", alias).strip()
        if alias and alias.lower() not in [x.lower() for x in seen]:
            seen.append(alias)
    return seen


def lower_initial(value: str) -> str:
    if not value:
        return value
    first = value.split(" ", 1)[0]
    if first.isupper():
        return value
    return value[0].lower() + value[1:]


def definition_from_extract(title: str, extract: str) -> str:
    sentence = first_sentence(extract)
    for alias in title_aliases(title):
        alias_re = re.escape(alias).replace(r"\ ", r"\s+")
        pattern = (
            r"^(?:in\s+[^,]+,\s+)?(?:a|an|the)?\s*"
            + alias_re
            + r"\s+(?:is|are|was|were|refers\s+to|denotes|means)\s+(.+)$"
        )
        match = re.match(pattern, sentence, flags=re.IGNORECASE)
        if match:
            desc = match.group(1).strip()
            desc = re.sub(r"^(?:a|an|the)\s+", "", desc, flags=re.IGNORECASE)
            return fit_string(lower_initial(desc.rstrip(".")))
    return fit_string(sentence.rstrip("."))


def strip_balanced(text: str, opener: str, closer: str) -> str:
    out: list[str] = []
    depth = 0
    i = 0
    while i < len(text):
        if text.startswith(opener, i):
            depth += 1
            i += len(opener)
            continue
        if depth and text.startswith(closer, i):
            depth -= 1
            i += len(closer)
            continue
        if depth == 0:
            out.append(text[i])
        i += 1
    return "".join(out)


def convert_links(text: str) -> str:
    def repl(match: re.Match[str]) -> str:
        raw = match.group(1)
        target = raw.split("|", 1)[0].strip()
        if re.match(r"(?i)^(file|image|category):", target):
            return ""
        return raw.split("|")[-1].strip()

    text = re.sub(r"\[\[([^\[\]]+)\]\]", repl, text)
    text = re.sub(r"\[(https?://\S+)\s+([^\]]+)\]", r"\2", text)
    text = re.sub(r"\[(https?://[^\]]+)\]", r"\1", text)
    return text


def wikitext_to_markdown(title: str, wikitext: str, fallback: str) -> str:
    text = ascii_text(wikitext)
    text = re.sub(r"<!--.*?-->", "", text, flags=re.DOTALL)
    text = re.sub(r"<ref\b[^>/]*/>", "", text, flags=re.IGNORECASE)
    text = re.sub(r"<ref\b[^>]*>.*?</ref>", "", text, flags=re.IGNORECASE | re.DOTALL)
    text = strip_balanced(text, "{{", "}}")
    text = strip_balanced(text, "{|", "|}")
    lines: list[str] = []
    for raw in text.splitlines():
        line = raw.strip()
        if not line:
            if lines and lines[-1] != "":
                lines.append("")
            continue
        if line.startswith("__") and line.endswith("__"):
            continue
        if re.match(r"(?i)^#redirect\b", line):
            continue
        if line.startswith(("[[Category:", "[[File:", "[[Image:")):
            continue
        heading = re.match(r"^(=+)\s*(.*?)\s*\1$", line)
        if heading:
            level = min(len(heading.group(1)) + 1, 6)
            lines.append("#" * level + " " + ascii_text(heading.group(2)))
            continue
        line = convert_links(line)
        line = re.sub(r"'{2,5}", "", line)
        line = re.sub(r"<[^>]+>", "", line)
        line = re.sub(r"^\*+\s*", "- ", line)
        line = re.sub(r"^#+\s*", "1. ", line)
        line = re.sub(r"\s+", " ", line).strip()
        if line:
            lines.append(line)
    body = "\n".join(lines).strip()
    return body or ascii_text(fallback)


def page_url(summary: dict[str, Any], page: dict[str, Any], title: str, lang: str) -> str:
    urls = summary.get("content_urls") or {}
    desktop = urls.get("desktop") or {}
    if desktop.get("page"):
        return ascii_text(desktop["page"])
    if page.get("fullurl"):
        return ascii_text(page["fullurl"])
    encoded = urllib.parse.quote(title.replace(" ", "_"), safe="/")
    return f"https://{lang}.wikipedia.org/wiki/{encoded}"


def revision_id(summary: dict[str, Any], page: dict[str, Any]) -> int:
    if isinstance(summary.get("revision"), int):
        return int(summary["revision"])
    revisions = page.get("revisions") or []
    if revisions and isinstance(revisions[0].get("revid"), int):
        return int(revisions[0]["revid"])
    return 0


def select_source(
    sources: list[dict[str, Any]], state: dict[str, Any], args: argparse.Namespace
) -> tuple[dict[str, Any], int | None]:
    if args.title:
        wanted = ascii_text(args.title).lower()
        for idx, source in enumerate(sources):
            if source["title"].lower() == wanted:
                return source, idx
        if not args.domain:
            die("--title outside sources.tsv requires --domain")
        return {"domain": p0_atom(args.domain), "title": ascii_text(args.title), "line": None}, None
    idx = args.source_index
    if idx is None:
        idx = int(state.get("next_index", 0) or 0)
    idx %= len(sources)
    return sources[idx], idx


def unique_key(title: str, domain: str, pages: dict[str, Any]) -> str:
    base = p0_atom(title)
    entry = pages.get(base)
    if not entry or entry.get("source_title") == title or entry.get("title") == title:
        return base
    base = p0_atom(f"{domain}_{title}")
    key = base
    counter = 2
    while pages.get(key) and pages[key].get("source_title") != title:
        key = f"{base}_{counter}"
        counter += 1
    return key


def make_event_id(processed_at: str, key: str, events: list[dict[str, Any]]) -> str:
    base = p0_atom(f"learn_{processed_at}_{key}")
    known = {event.get("event_id") for event in events}
    if base not in known:
        return base
    suffix = hashlib.sha1(f"{processed_at}:{key}".encode("utf-8")).hexdigest()[:8]
    return p0_atom(f"{base}_{suffix}")


def render_markdown(
    source: dict[str, Any],
    summary: dict[str, Any],
    wikitext: str,
    concept: str,
    url: str,
    rev: int,
    event_id: str,
    log_rel: str,
    processed_at: str,
) -> str:
    title = ascii_text(summary.get("title") or source["title"])
    extract = ascii_text(summary.get("extract") or "")
    body = wikitext_to_markdown(title, wikitext, extract)
    return (
        f"# {title}\n\n"
        f"- Learning event: `{event_id}`\n"
        f"- Operational log: `{log_rel}`\n"
        f"- Domain: `{source['domain']}`\n"
        f"- Source: {url}\n"
        f"- Wikipedia revision: `{rev}`\n"
        f"- Processed: `{processed_at}`\n\n"
        "## Learned Concept\n\n"
        f"{concept}\n\n"
        "## Extract\n\n"
        f"{extract}\n\n"
        "## Page Text\n\n"
        f"{body}\n"
    )


def update_learning_state(
    source: dict[str, Any],
    summary: dict[str, Any],
    page: dict[str, Any],
    wikitext: str,
    manifest: dict[str, Any],
    pages_dir: Path,
    logs_dir: Path,
    lang: str,
    processed_at: str,
    selected_index: int | None,
    next_index: int | None,
    kb_path: Path,
    manifest_path: Path,
    sources_path: Path,
    state_path: Path,
) -> tuple[str, dict[str, Any], dict[str, Any]]:
    pages = manifest.setdefault("pages", {})
    events = manifest.setdefault("events", [])
    key = unique_key(source["title"], source["domain"], pages)
    event_id = make_event_id(processed_at, key, events)
    title = ascii_text(summary.get("title") or source["title"])
    extract = ascii_text(summary.get("extract") or "")
    concept = definition_from_extract(title, extract)
    if not concept:
        die(f"could not extract a concept from {source['title']}")
    url = page_url(summary, page, source["title"], lang)
    rev = revision_id(summary, page)
    markdown_path = pages_dir / f"{key}.md"
    log_path = logs_dir / f"{event_id}.json"
    markdown_rel = relative(markdown_path)
    log_rel = relative(log_path)

    markdown = render_markdown(
        source,
        summary,
        wikitext,
        concept,
        url,
        rev,
        event_id,
        log_rel,
        processed_at,
    )
    write_text_if_changed(markdown_path, markdown)

    entry = {
        "concept": concept,
        "description": fit_string(summary.get("description") or "", 80),
        "domain": source["domain"],
        "extract_sha256": hashlib.sha256(extract.encode("utf-8")).hexdigest(),
        "key": key,
        "last_event_id": event_id,
        "markdown_path": markdown_rel,
        "pageid": int(summary.get("pageid") or page.get("pageid") or 0),
        "processed_at": processed_at,
        "revision": rev,
        "source_title": source["title"],
        "title": title,
        "url": url,
    }
    pages[key] = entry

    event_summary = f"learned {key}: {concept}"
    event = {
        "concept": concept,
        "domain": source["domain"],
        "event_id": event_id,
        "key": key,
        "log_path": log_rel,
        "markdown_path": markdown_rel,
        "processed_at": processed_at,
        "prompt": f"what do you know about {event_id}",
        "revision": rev,
        "source_index": selected_index,
        "source_line": source.get("line"),
        "source_title": source["title"],
        "summary": fit_string(event_summary),
        "title": title,
        "url": url,
    }
    events.append(event)

    log = {
        "event_id": event_id,
        "extraction": {
            "concept": concept,
            "concept_key": key,
            "extract_sha256": entry["extract_sha256"],
            "method": "deterministic_first_sentence_v1",
            "no_ai": True,
        },
        "files": {
            "kb": relative(kb_path),
            "log": log_rel,
            "manifest": relative(manifest_path),
            "markdown": markdown_rel,
            "sources": relative(sources_path),
            "state": relative(state_path),
        },
        "operational": {
            "next_index": next_index,
            "status": "ok",
            "user_agent": USER_AGENT,
        },
        "processed_at": processed_at,
        "prompt": {"extract_session": f"what do you know about {event_id}"},
        "source": {
            "domain": source["domain"],
            "index": selected_index,
            "line": source.get("line"),
            "title": source["title"],
        },
        "wikipedia": {
            "canonical_title": title,
            "lang": lang,
            "pageid": entry["pageid"],
            "revision": rev,
            "url": url,
        },
    }
    write_json(log_path, log)
    return key, entry, event


def source_order(sources: list[dict[str, Any]], pages: dict[str, Any]) -> list[dict[str, Any]]:
    ordered: list[dict[str, Any]] = []
    used: set[str] = set()
    for source in sources:
        for key, entry in pages.items():
            if (
                entry.get("domain") == source["domain"]
                and (
                    entry.get("source_title") == source["title"]
                    or entry.get("title") == source["title"]
                )
                and key not in used
            ):
                ordered.append(entry)
                used.add(key)
                break
    extras = [entry for key, entry in pages.items() if key not in used]
    extras.sort(key=lambda e: (e.get("domain", ""), e.get("title", "")))
    ordered.extend(extras)
    return ordered


def render_learned_p0(sources: list[dict[str, Any]], manifest: dict[str, Any]) -> str:
    pages = manifest.get("pages") or {}
    events = manifest.get("events") or []
    lines = [
        "% Generated by scripts/learn.py from curated Wikipedia pages.",
        "% Do not hand-edit generated facts; edit kb/learning/sources.tsv instead.",
        "% No AI is used: each wiki_concept comes from a deterministic summary rule.",
        "% Learning events connect concepts, Markdown snapshots, and operational logs.",
        "",
    ]
    for entry in source_order(sources, pages):
        key = p0_atom(entry.get("key", "concept"))
        domain = p0_atom(entry.get("domain", "general"))
        title = ascii_text(entry.get("title", key))
        url = ascii_text(entry.get("url", ""))
        rev = int(entry.get("revision") or 0)
        processed_at = ascii_text(entry.get("processed_at", ""))
        concept = entry.get("concept") or ""
        lines.append(f"% {domain} / {title}")
        if url:
            lines.append(f"% source: {url}")
        lines.append(f"% revision: {rev}; processed: {processed_at}")
        lines.append(f"wiki_concept({key}, {domain}, {p0_string(concept)}).")
        lines.append("")

    if events:
        lines.append("% --- learning event audit trail ---")
        for event in events:
            event_id = p0_atom(event.get("event_id", "learning_event"))
            key = p0_atom(event.get("key", "concept"))
            domain = p0_atom(event.get("domain", "general"))
            rev = int(event.get("revision") or 0)
            rev_atom = p0_atom(f"rev_{rev}") if rev else "rev_unknown"
            lines.append(f"% event {event_id}")
            lines.append(f"learning_event({event_id}, {p0_string(event.get('summary', 'learning event'))}).")
            lines.append(f"learning_event_time({event_id}, {p0_string(event.get('processed_at', ''))}).")
            lines.append(f"learning_event_domain({event_id}, {domain}).")
            lines.append(f"learning_event_concept({event_id}, {key}).")
            lines.append(f"learning_event_revision({event_id}, {rev_atom}).")
            lines.append(f"learning_event_markdown({event_id}, {p0_string(event.get('markdown_path', ''))}).")
            lines.append(f"learning_event_log({event_id}, {p0_string(event.get('log_path', ''))}).")
            lines.append("")
    return "\n".join(lines).rstrip() + "\n"


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sources", type=Path, default=DEFAULT_SOURCES)
    parser.add_argument("--kb", type=Path, default=DEFAULT_KB)
    parser.add_argument("--state", type=Path, default=DEFAULT_STATE)
    parser.add_argument("--manifest", type=Path, default=DEFAULT_MANIFEST)
    parser.add_argument("--pages-dir", type=Path, default=DEFAULT_PAGES_DIR)
    parser.add_argument("--logs-dir", type=Path, default=DEFAULT_LOGS_DIR)
    parser.add_argument("--lang", default="en")
    parser.add_argument("--timeout", type=int, default=30)
    parser.add_argument("--source-index", type=int)
    parser.add_argument("--title")
    parser.add_argument("--domain")
    parser.add_argument("--fixture", type=Path, help="offline fixture JSON for tests")
    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_arg_parser().parse_args(argv)
    sources = read_sources(args.sources)
    state = load_json(args.state, {"next_index": 0})
    manifest = load_json(args.manifest, {"version": 1, "pages": {}, "events": []})
    manifest.setdefault("version", 1)
    manifest.setdefault("pages", {})
    manifest.setdefault("events", [])

    source, selected_index = select_source(sources, state, args)
    next_index = None
    if selected_index is not None:
        next_index = (selected_index + 1) % len(sources)

    processed_at = utc_now()
    if args.fixture:
        summary, wikitext, page = load_fixture(args.fixture)
    else:
        summary = fetch_summary(source["title"], args.lang, args.timeout)
        wikitext, page = fetch_wikitext(source["title"], args.lang, args.timeout)

    key, entry, event = update_learning_state(
        source,
        summary,
        page,
        wikitext,
        manifest,
        args.pages_dir,
        args.logs_dir,
        args.lang,
        processed_at,
        selected_index,
        next_index,
        args.kb,
        args.manifest,
        args.sources,
        args.state,
    )
    write_text_if_changed(args.kb, render_learned_p0(sources, manifest))
    write_json(args.manifest, manifest)

    if next_index is not None:
        state["next_index"] = next_index
    state["last_processed"] = {
        "domain": entry["domain"],
        "event_id": event["event_id"],
        "key": key,
        "log_path": event["log_path"],
        "markdown_path": event["markdown_path"],
        "processed_at": processed_at,
        "prompt": event["prompt"],
        "revision": entry["revision"],
        "title": entry["title"],
    }
    write_json(args.state, state)

    print(
        "learned "
        f"{entry['domain']}/{entry['title']} -> {key} "
        f"via {event['event_id']} "
        f"(revision {entry['revision']}, {event['markdown_path']})"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
