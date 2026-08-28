/*
 * mcp.h - parrot0's MCP (Model Context Protocol) engine (gen277).
 *
 * `parrot0 --mcp-engine` runs a JSON-RPC 2.0 server over stdio that exposes the
 * Prolog-like inference engine and the generation primitives as MCP tools, so an
 * external agent can drive parrot0's KB like an inference database: write
 * knowledge, read it, infer over it, generate, set the style temperature, and
 * extract knowledge from text. State persists for the whole session (one process
 * = one Brain), so an agent that asserts a fact sees it on the next call; a
 * `kb.save` + `kb.restore` (the /restore of gen276, as a tool) makes knowledge
 * written to a file go live without restarting. See docs/plans/mcp-engine.md.
 */
#ifndef PARROT0_MCP_H
#define PARROT0_MCP_H

#include "brain.h"

/* Serve MCP JSON-RPC over stdin/stdout (newline-delimited messages) until EOF.
 * Returns 0 on clean shutdown. The brain is owned by the caller. */
int mcp_serve_stdio(Brain *brain);

/* Invoke ONE MCP tool against an existing brain, without a JSON-RPC round trip.
 *
 * Il layer MCP era verificabile solo da fuori, a colpi di JSON-RPC su un
 * processo separato: e' il motivo per cui una dozzina di suite vive ancora in
 * shell invece che in .p0t. Questa e' la stessa strada che percorre
 * `tools/call`, aperta a chi il brain ce l'ha gia' in mano — cosi' `!mcp` nel
 * test-engine prova il MCP vero e non una sua imitazione.
 *
 * `args_json` e' l'oggetto degli argomenti ("{\"pred\":\"dog\",…}"), o NULL.
 * Scrive in `out` il payload JSON dello strumento. Ritorna 1 se lo strumento
 * esiste ed e' stato eseguito, 0 altrimenti (e `out` porta l'errore). */
int mcp_tool_invoke(Brain *brain, const char *name, const char *args_json,
                    char *out, size_t outsz);

#endif /* PARROT0_MCP_H */