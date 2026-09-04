# Local event journal

The public Python API is `Journal(path).append/load/compact`. Operations also
use `cli.py`; deployments are Unix-like and provide `fcntl.flock`.

The current implementation predates concurrent ingestion and crash-safe
compaction. Incident CA-219 is the acceptance contract supplied with the match.
