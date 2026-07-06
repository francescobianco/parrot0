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

#endif /* PARROT0_MCP_H */