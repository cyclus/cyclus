"""Generate injectable test type registrations; pass the output header path."""

import argparse
import json
import sys
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output", type=Path, help="generated C++ header path")
    args = parser.parse_args()

    # Locate inputs from this script, independent of the build working directory.
    source_root = Path(__file__).resolve().parents[2]
    sys.path.insert(0, str(source_root / "cli"))
    from cycpp import parse_template

    rows = json.loads((source_root / "share" / "dbtypes.json").read_text())[1:]
    # The table includes historical versions and fixed/variable-length duplicates.
    version = max(tuple(map(int, r[5].lstrip("v").split("."))) for r in rows
                  if r[4] == "SQLite")
    types = sorted({r[2] for r in rows if r[4] == "SQLite" and r[6]
                    and tuple(map(int, r[5].lstrip("v").split("."))) == version})
    lines = ["// Generated from share/dbtypes.json; do not edit."]
    for cpp in types:
        annotation = parse_template(cpp) if "<" in cpp else cpp
        lines.append('Register<%s>(registry, R"type(%s)type");' %
                     (cpp, json.dumps(annotation)))
    args.output.write_text("\n".join(lines) + "\n")


if __name__ == "__main__":
    main()
