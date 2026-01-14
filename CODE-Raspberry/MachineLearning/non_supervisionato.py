import numpy as np
import matplotlib.pyplot as plt

from sklearn.datasets import load_iris
from sklearn.preprocessing import StandardScaler
from sklearn.cluster import KMeans
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

iris = load_iris()
X = iris.data

scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

kmeans = KMeans(n_clusters=3, random_state=42, n_init=10)
clusters = kmeans.fit_predict(X_scaled)

pca = PCA(n_components=2, random_state=42)
X_2d = pca.fit_transform(X_scaled)

# Centroidi in PCA (prendendo i centroidi nello spazio scalato)
centroids_2d = pca.transform(kmeans.cluster_centers_)

plt.figure()
plt.scatter(X_2d[:, 0], X_2d[:, 1], c=clusters, alpha=0.8)
plt.scatter(centroids_2d[:, 0], centroids_2d[:, 1], marker="X", s=200, label="Centroids")
plt.title("Unsupervised: KMeans clusters in PCA(2D)")
plt.xlabel("PC1")
plt.ylabel("PC2")
plt.legend()
save_plot("out/unsupervised_kmeans_pca.png", open_in_windows=False)

print("Saved plot in out/: unsupervised_kmeans_pca.png")
