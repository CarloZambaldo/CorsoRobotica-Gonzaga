import os
import subprocess
from pathlib import Path

def save_plot(filename: str, open_in_windows: bool = True, dpi: int = 150):
	Path(filename).parent.mkdir(parents=True, exist_ok=True)
	import matplotlib.pyplot as plt
	plt.savefig(filename, dpi=dpi, bbox_inches="tight")
	plt.close()

	# Se sei in WSL, puoi aprire il file con explorer.exe
	if open_in_windows:
		try:
			subprocess.run(["explorer.exe", str(Path(filename).resolve())], check=False)
		except Exception:
			pass
