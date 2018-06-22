import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
TITLE = "block8_step4"
x = np.linspace(1,299,299)

reader = pd.read_csv(TITLE+".csv")
fs = reader['FS'][:299]
tss = reader['TSS'][:299]
os = reader['OS'][:299]
csa = reader['CSA'][:299]
tls = reader['TLS'][:299]

line1 = plt.plot(x, fs, 'r', label="Full Search")
line2 = plt.plot(x, tss, 'g', label="Three Step Search")
line3 = plt.plot(x, os, 'c', label="Orthogonal Search")
line4 = plt.plot(x, csa, 'y', label="Cross Search")
line5 = plt.plot(x, tls, 'k', label="Two logarithm Search")
plt.title("Akiyo.QCIF")
plt.xlabel("Frame")
plt.ylabel("PSNR")
plt.legend(loc="best")
plt.savefig(TITLE+"_Akiyo.png")
# plt.show()
plt.gcf().clear()

fs = reader['FS'][299:598]
tss = reader['TSS'][299:598]
os = reader['OS'][299:598]
csa = reader['CSA'][299:598]
tls = reader['TLS'][299:598]

line1 = plt.plot(x, fs, 'r', label="Full Search")
line2 = plt.plot(x, tss, 'g', label="Three Step Search")
line3 = plt.plot(x, os, 'c', label="Orthogonal Search")
line4 = plt.plot(x, csa, 'y', label="Cross Search")
line5 = plt.plot(x, tls, 'k', label="Two logarithm Search")
plt.title("Table.QCIF")
plt.xlabel("Frame")
plt.ylabel("PSNR")
plt.legend(loc="best")
plt.savefig(TITLE+"_Table.png")
plt.gcf().clear()

fs = reader['FS'][598:897]
tss = reader['TSS'][598:897]
os = reader['OS'][598:897]
csa = reader['CSA'][598:897]
tls = reader['TLS'][598:897]

line1 = plt.plot(x, fs, 'r', label="Full Search")
line2 = plt.plot(x, tss, 'g', label="Three Step Search")
line3 = plt.plot(x, os, 'c', label="Orthogonal Search")
line4 = plt.plot(x, csa, 'y', label="Cross Search")
line5 = plt.plot(x, tls, 'k', label="Two logarithm Search")
plt.title("Foreman.CIF")
plt.xlabel("Frame")
plt.ylabel("PSNR")
plt.legend(loc="best")
plt.savefig(TITLE+"_Foreman.png")
plt.gcf().clear()

fs = reader['FS'][897:]
tss = reader['TSS'][897:]
os = reader['OS'][897:]
csa = reader['CSA'][897:]
tls = reader['TLS'][897:]

line1 = plt.plot(x, fs, 'r', label="Full Search")
line2 = plt.plot(x, tss, 'g', label="Three Step Search")
line3 = plt.plot(x, os, 'c', label="Orthogonal Search")
line4 = plt.plot(x, csa, 'y', label="Cross Search")
line5 = plt.plot(x, tls, 'k', label="Two logarithm Search")
plt.title("Stefan.CIF")
plt.xlabel("Frame")
plt.ylabel("PSNR")
plt.legend(loc="best")
plt.savefig(TITLE+"_Stefan.png")
plt.gcf().clear()