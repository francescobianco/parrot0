#!/usr/bin/env python3
# gen223: prepare ~/.pi/agent/models.json for the parrot0 provider WITHOUT clobbering
# any other providers the user already registered. Merges the `parrot0` entry in place.
# Usage: pi_setup.py <baseUrl>      (e.g. http://127.0.0.1:9902/v1)
# Honors PI_MODELS_JSON to override the target file.
import json, os, sys

base_url = sys.argv[1] if len(sys.argv) > 1 else "http://127.0.0.1:9902/v1"
path = os.environ.get("PI_MODELS_JSON",
                      os.path.expanduser("~/.pi/agent/models.json"))

provider = {
    "baseUrl": base_url,
    "api": "openai-completions",
    "apiKey": "parrot0",
    "compat": {
        "supportsDeveloperRole": False,
        "supportsReasoningEffort": False,
        "supportsUsageInStreaming": False,
    },
    "models": [{
        "id": "parrot0",
        "name": "parrot0 (pure C agent)",
        "reasoning": False,
        "input": ["text"],
        "cost": {"input": 0, "output": 0, "cacheRead": 0, "cacheWrite": 0},
        "contextWindow": 32000,
        "maxTokens": 4096,
    }],
}

cfg = {}
if os.path.exists(path):
    try:
        with open(path) as f:
            cfg = json.load(f)
    except (json.JSONDecodeError, OSError) as e:
        print(f"pi_setup: existing {path} is unreadable ({e}); refusing to overwrite",
              file=sys.stderr)
        sys.exit(1)
if not isinstance(cfg, dict):
    cfg = {}
cfg.setdefault("providers", {})
cfg["providers"]["parrot0"] = provider

os.makedirs(os.path.dirname(path), exist_ok=True)
with open(path, "w") as f:
    json.dump(cfg, f, indent=2)
    f.write("\n")

others = [p for p in cfg["providers"] if p != "parrot0"]
print(f"pi config: {path}  (provider 'parrot0' -> {base_url}"
      + (f"; preserved: {', '.join(others)}" if others else "") + ")")