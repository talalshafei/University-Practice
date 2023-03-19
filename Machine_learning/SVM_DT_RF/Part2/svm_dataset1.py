"""
Name: Talal Shafei
ID: 2542371
"""

import pickle
import matplotlib.pyplot as plt
from sklearn.svm import SVC
from sklearn.inspection import DecisionBoundaryDisplay

dataset, labels = pickle.load(open("../data/part2_dataset1.data", "rb"))

hyper = [(kernel, C) for kernel in ["rbf", "poly"] for C in [0.1, 1.0]]
plt.style.use('bmh')
for kernel, C in hyper:
    # training
    svc_model = SVC(kernel=kernel, C=C)
    svc_model.fit(dataset, labels)

    # plotting
    disp = DecisionBoundaryDisplay.from_estimator(
        svc_model,
        dataset,
        response_method="predict",
        alpha=0.5,
        cmap="Paired_r"
    )
    scatter = disp.ax_.scatter(dataset[:, 0], dataset[:, 1], c=labels, cmap="Paired_r", edgecolors="k")
    disp.ax_.set_title(f"For kernel: {kernel}, and C: {C:.1f}", fontsize=17)
    disp.ax_.set_xlabel("First feature", fontsize=15)
    disp.ax_.set_ylabel("Second feature", fontsize=15)
    legend1 = disp.ax_.legend(*scatter.legend_elements(), title="Classes")
    disp.ax_.add_artist(legend1)
    # plt.savefig(f"figures/{kernel}_{C}.png")
    plt.show()
