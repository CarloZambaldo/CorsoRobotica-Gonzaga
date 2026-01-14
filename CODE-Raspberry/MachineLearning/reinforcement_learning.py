import numpy as np
import matplotlib.pyplot as plt

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

# --- Ambiente GridWorld ---
H, W = 5, 5
START = (0, 0)
GOAL  = (4, 4)

A_UP, A_DOWN, A_LEFT, A_RIGHT = 0, 1, 2, 3
n_actions = 4

def to_state(pos):
	return pos[0] * W + pos[1]

def step(pos, action):
	r, c = pos
	if action == A_UP:
		r = max(0, r - 1)
	elif action == A_DOWN:
		r = min(H - 1, r + 1)
	elif action == A_LEFT:
		c = max(0, c - 1)
	elif action == A_RIGHT:
		c = min(W - 1, c + 1)

	new_pos = (r, c)
	if new_pos == GOAL:
		return new_pos, 10.0, True
	return new_pos, -1.0, False

# --- Q-learning ---
n_states = H * W
Q = np.zeros((n_states, n_actions), dtype=float)

alpha = 0.2
gamma = 0.95
eps = 0.2
episodes = 800
max_steps = 200

rng = np.random.default_rng(42)

episode_returns = []

for ep in range(episodes):
	pos = START
	done = False
	ret = 0.0
	steps = 0

	while not done and steps < max_steps:
		s = to_state(pos)

		if rng.random() < eps:
			a = int(rng.integers(0, n_actions))
		else:
			a = int(np.argmax(Q[s]))

		new_pos, r, done = step(pos, a)
		s2 = to_state(new_pos)

		# target Q-learning
		best_next = np.max(Q[s2])
		td_target = r + gamma * best_next * (0.0 if done else 1.0)
		Q[s, a] = (1 - alpha) * Q[s, a] + alpha * td_target

		pos = new_pos
		ret += r
		steps += 1

	episode_returns.append(ret)

episode_returns = np.array(episode_returns)

# --- Plot 1: Learning curve ---
plt.figure()
plt.plot(episode_returns, label="Episode return")
# media mobile semplice
window = 30
if len(episode_returns) >= window:
	ma = np.convolve(episode_returns, np.ones(window)/window, mode="valid")
	plt.plot(np.arange(window-1, episodes), ma, label=f"Moving avg ({window})")
plt.title("RL: Q-learning training curve")
plt.xlabel("Episode")
plt.ylabel("Return (sum of rewards)")
plt.legend()
save_plot("out/rl_rewards.png", open_in_windows=False)

# --- Estrai policy greedy ---
policy = np.argmax(Q, axis=1).reshape(H, W)

# --- Plot 2: Policy grid (frecce) ---
arrow = {
	A_UP: "↑",
	A_DOWN: "↓",
	A_LEFT: "←",
	A_RIGHT: "→"
}

plt.figure()
plt.imshow(np.zeros((H, W)))  # sfondo
for r in range(H):
	for c in range(W):
		if (r, c) == GOAL:
			txt = "G"
		elif (r, c) == START:
			txt = "S"
		else:
			txt = arrow[int(policy[r, c])]
		plt.text(c, r, txt, ha="center", va="center", fontsize=16)

plt.title("RL: Greedy policy after training")
plt.xticks(range(W))
plt.yticks(range(H))
plt.gca().invert_yaxis()
save_plot("out/rl_policy_grid.png", open_in_windows=False)

# --- Simula path con policy greedy ---
pos = START
path = [pos]
for _ in range(100):
	s = to_state(pos)
	a = int(np.argmax(Q[s]))
	pos, _, done = step(pos, a)
	path.append(pos)
	if done:
		break

# --- Plot 3: Path sulla griglia ---
grid = np.zeros((H, W), dtype=float)
plt.figure()
plt.imshow(grid)
# path in coordinate (c, r) per scatter
xs = [p[1] for p in path]
ys = [p[0] for p in path]
plt.plot(xs, ys, marker="o")
plt.scatter([START[1]], [START[0]], marker="s", s=150, label="Start")
plt.scatter([GOAL[1]], [GOAL[0]], marker="*", s=200, label="Goal")
plt.title(f"RL: Path from S to G (steps={len(path)-1})")
plt.xticks(range(W))
plt.yticks(range(H))
plt.gca().invert_yaxis()
plt.legend()
save_plot("out/rl_path.png", open_in_windows=False)

print("Saved plots in out/: rl_rewards.png, rl_policy_grid.png, rl_path.png")
print("Path:", path)
