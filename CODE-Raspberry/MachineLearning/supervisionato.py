import numpy as np
import matplotlib.pyplot as plt

from sklearn.datasets import load_iris
from sklearn.model_selection import train_test_split
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import StandardScaler
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import confusion_matrix, ConfusionMatrixDisplay, accuracy_score
from sklearn.decomposition import PCA

import subprocess
from pathlib import Path

def save_plot(filename: str, open_in_windows: bool = True, dpi: int = 150):
	Path(filename).parent.mkdir(parents=True, exist_ok=True)
	plt.savefig(filename, dpi=dpi, bbox_inches="tight")
	plt.close()
	if open_in_windows:
		try:
			subprocess.run(["explorer.exe", str(Path(filename).resolve())], check=False)
		except Exception:
			pass

# --- Dati ---
iris = load_iris()
X, y = iris.data, iris.target
class_names = iris.target_names

X_train, X_test, y_train, y_test = train_test_split(
	X, y, test_size=0.2, random_state=42, stratify=y
)

# --- Modello ---
model = make_pipeline(
	StandardScaler(),
	LogisticRegression(max_iter=1000)
)
model.fit(X_train, y_train)

y_pred = model.predict(X_test)
acc = accuracy_score(y_test, y_pred)
print("Accuracy:", acc)

# --- Plot 1: Confusion matrix ---
cm = confusion_matrix(y_test, y_pred, labels=[0, 1, 2])
disp = ConfusionMatrixDisplay(confusion_matrix=cm, display_labels=class_names)

plt.figure()
disp.plot(values_format="d")
plt.title(f"Supervised: Confusion Matrix (acc={acc:.3f})")
save_plot("out/supervised_confusion.png", open_in_windows=False)

# --- Plot 2: PCA 2D con true vs pred (corretto/errato) ---
# PCA sul dataset completo per una visualizzazione coerente
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)
X_2d = PCA(n_components=2, random_state=42).fit_transform(X_scaled)

# Indici test per posizionare i punti test nello spazio PCA
# (ricaviamo i punti test scalati usando lo stesso scaler)
X_test_scaled = scaler.transform(X_test)
X_test_2d = PCA(n_components=2, random_state=42).fit(X_scaled).transform(X_test_scaled)

correct = (y_pred == y_test)

plt.figure()
# punti test colorati per classe vera
for k in [0, 1, 2]:
	mask = (y_test == k)
	plt.scatter(
		X_test_2d[mask, 0], X_test_2d[mask, 1],
		label=f"True: {class_names[k]}",
		alpha=0.8
	)

# evidenzia errori con un contorno (marker 'x')
plt.scatter(
	X_test_2d[~correct, 0], X_test_2d[~correct, 1],
	marker="x",
	label="Misclassified",
)

plt.title("Supervised: Test set in PCA(2D)\nColor=True class, X=misclassified")
plt.xlabel("PC1")
plt.ylabel("PC2")
plt.legend(loc="best")
save_plot("out/supervised_pca_true_pred.png", open_in_windows=False)

print("Saved plots in out/: supervised_confusion.png, supervised_pca_true_pred.png")
