#!/usr/bin/env python3
"""
Parse icru_90_raw.txt (pdftotext output of ICRU Report 90 appendix tables)
and emit intermediate .dat / *Eng.dat files compatible with dat2c.py.

Character-encoding fixes applied to each data line
---------------------------------------------------
pdftotext renders scientific notation with two artefacts:
  1. Plus signs in exponents become thorn (þ, U+00FE):
         1.025Eþ02  →  1.025E+02
  2. Minus signs in exponents become '2' followed by the two exponent digits:
         2.883E203  →  2.883E-03
  3. Large numbers use a thousands-separator space:
         10 000.0000  →  10000.0000

Only the first two columns of each data row are extracted:
  col 0 — T  (kinetic energy, MeV, same unit for all ICRU 90 tables)
  col 1 — Sel/r  (electronic mass stopping power, MeV cm² g⁻¹)

Output files
------------
  ICRU90_e.dat   / icru90_eEng.dat    electrons  (ion id -1)
  ICRU90_pos.dat / icru90_posEng.dat  positrons  (ion id -2)
  ICRU90_p.dat   / icru90_pEng.dat    protons    (ion id  1)
  ICRU90_a.dat   / icru90_aEng.dat    alphas     (ion id  2)
  ICRU90_C.dat   / icru90_CEng.dat    carbon     (ion id  6)

Energy scale notes for dat2c.py
--------------------------------
All ICRU 90 tables list T as total kinetic energy in MeV.  dat2c.py applies
an energy_scale factor to convert to MeV/u before embedding:
  electrons/positrons : 1.0  (no nucleons; treat MeV as MeV/u)
  proton              : 1.0  (A = 1)
  alpha               : 0.25 (A = 4)
  carbon              : 1/12 (A = 12)

Usage:
    python3 libdedx/data/tools/pdf2dat.py
"""

import os
import re
import sys
from collections import defaultdict

# ---------------------------------------------------------------------------
# Ion-id and target-id mappings
# ---------------------------------------------------------------------------

PARTICLE_ION = {
    "electrons":        -1,
    "positrons":        -2,
    "protons":           1,
    "alpha particles":   2,
    "carbon ions":       6,
}

MATERIAL_TARGET = {
    "air":          104,
    "graphite":     906,
    "liquid water": 276,
}

ION_STEM = {
    -1: "e",
    -2: "pos",
     1: "p",
     2: "a",
     6: "C",
}

# ---------------------------------------------------------------------------
# pdftotext artefact fixes
# ---------------------------------------------------------------------------

def fix_line(line):
    """Fix pdftotext mangling in a data line before numeric parsing."""
    # 1. Thorn → plus  (e.g. Eþ02 → E+02)
    line = line.replace("\u00fe", "+")
    # 2. Garbled minus in two-digit exponent: E2NN → E-NN
    #    (pdftotext drops the minus and left-shifts the digit pair)
    line = re.sub(r'E2(\d{2})', r'E-\1', line)
    # 3. Thousands-separator space inside a number: "10 000.0000" → "10000.0000"
    line = re.sub(r'(\d) (\d{3}\.)', r'\1\2', line)
    return line

# ---------------------------------------------------------------------------
# Table header detection
# ---------------------------------------------------------------------------

_TABLE_RE = re.compile(
    r'Table\s+A\.\d+\.\s+(.+?)\s+in\s+([^,]+)',
    re.IGNORECASE,
)

def parse_header(line):
    """Return (ion_id, target_id) if line is a table header, else None."""
    m = _TABLE_RE.search(line)
    if not m:
        return None
    particle = m.group(1).strip().lower()
    material = m.group(2).strip().lower()
    ion_id    = PARTICLE_ION.get(particle)
    target_id = MATERIAL_TARGET.get(material)
    if ion_id is None or target_id is None:
        print(
            f"WARNING: unrecognised particle={particle!r} "
            f"or material={material!r}",
            file=sys.stderr,
        )
        return None
    return ion_id, target_id

# ---------------------------------------------------------------------------
# Main parser
# ---------------------------------------------------------------------------

def is_data_line(line):
    """True when the stripped line looks like a table data row."""
    return bool(re.match(r'\d', line.strip()))


def parse_file(path):
    """
    Read icru_90_raw.txt and return:
        { ion_id: { target_id: [ (T, sel_r), … ] } }
    """
    data = defaultdict(lambda: defaultdict(list))
    current_ion    = None
    current_target = None

    with open(path, encoding="utf-8") as fh:
        for raw in fh:
            hdr = parse_header(raw)
            if hdr is not None:
                current_ion, current_target = hdr
                continue

            if current_ion is None:
                continue
            if not is_data_line(raw):
                continue

            line   = fix_line(raw)
            tokens = line.split()
            if len(tokens) < 2:
                continue

            try:
                T     = float(tokens[0])
                sel_r = float(tokens[1])
            except ValueError:
                continue

            data[current_ion][current_target].append((T, sel_r))

    return data

# ---------------------------------------------------------------------------
# Output writers
# ---------------------------------------------------------------------------

def write_dataset(ion_id, target_data, outdir):
    """Write ICRU90_<stem>.dat and icru90_<stem>Eng.dat."""
    stem     = ION_STEM[ion_id]
    dat_path = os.path.join(outdir, f"ICRU90_{stem}.dat")
    eng_path = os.path.join(outdir, f"icru90_{stem}Eng.dat")

    # Collect and verify that all targets share the same energy grid.
    ref_grid = None
    for target_id, pairs in sorted(target_data.items()):
        grid = [T for T, _ in pairs]
        if ref_grid is None:
            ref_grid = grid
        elif len(grid) != len(ref_grid) or any(
            abs(a - b) > 1e-9 for a, b in zip(grid, ref_grid)
        ):
            print(
                f"WARNING: energy grid mismatch for ion={ion_id} target={target_id}",
                file=sys.stderr,
            )

    n_pts = len(ref_grid)

    # Energy file
    with open(eng_path, "w") as fh:
        fh.write(f"{n_pts}\n")
        for T in ref_grid:
            fh.write(f"{T:.4E}\n")

    # Dat file
    with open(dat_path, "w") as fh:
        for target_id, pairs in sorted(target_data.items()):
            fh.write(f"#{target_id}:{ion_id}:{n_pts}\n")
            for _, sel_r in pairs:
                fh.write(f"{sel_r:.6E}\n")

    print(
        f"  {os.path.basename(dat_path):30s}  "
        f"{n_pts} energies, "
        f"{len(target_data)} targets: "
        + ", ".join(str(t) for t in sorted(target_data))
    )
    print(f"  {os.path.basename(eng_path)}")

# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir   = os.path.dirname(script_dir)
    raw_dir    = os.path.join(data_dir, "raw")
    raw_path   = os.path.join(raw_dir, "icru_90_raw.txt")

    if not os.path.exists(raw_path):
        print(f"ERROR: {raw_path} not found", file=sys.stderr)
        sys.exit(1)

    print(f"Parsing {raw_path} ...")
    data = parse_file(raw_path)

    if not data:
        print("ERROR: no data extracted — check table headers in raw file.", file=sys.stderr)
        sys.exit(1)

    for ion_id in sorted(data):
        write_dataset(ion_id, data[ion_id], raw_dir)

    print("Done.")


if __name__ == "__main__":
    main()
