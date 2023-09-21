import matplotlib.pyplot as plt
import pandas as pd
import argparse
import os
import sys


parser = argparse.ArgumentParser()

parser.add_argument("-i", "--input", dest="input", help="input file", type=str)
parser.add_argument("-o", "--output", dest="output", help="output file", type=str)
parser.add_argument("-x", "--xlabel", dest="xlabel", help="xlabel", type=str)
parser.add_argument("-y", "--ylabel", dest="ylabel", help="ylabel", type=str)
parser.add_argument("-t", "--title", dest="title", help="title", type=str)
parser.add_argument("-l", "--logscale", dest="logscale", help="logscale", type=str)


options = parser.parse_args()

if options.input is None:
    parser.print_usage()
    sys.exit()


data = pd.read_csv(options.input, delimiter=";")
print(f"Column labels: {data.columns[0]}")


# Uses the first column for the x axes
ax = data.plot(x=data.columns[0], marker="o", xticks=data.iloc[:,0])

# Set the bottom value to 0 for the Y axes
ax.set_ylim(bottom=0)

if options.title is None:
    ax.set_title("Plot", fontsize="x-large")
else:
    ax.set_title(options.title, fontsize="x-large")

if options.xlabel is None:
    ax.set_xlabel("Problem size", fontsize="x-large")
else:
    ax.set_xlabel(options.xlabel, fontsize="x-large")

if options.ylabel is None: 
    ax.set_ylabel("Execution time in ns", fontsize="x-large")
else:
    ax.set_ylabel(options.ylabel, fontsize="x-large")

if options.logscale=="x": 
   ax.set_xscale("log")
elif options.logscale=="y":
    ax.set_yscale("log")
elif options.logscale=="xy":
    ax.set_xscale("log")
    ax.set_yscale("log")

# setting font sizes
ax.legend(fontsize="x-large")
plt.yticks(fontsize="x-large")
plt.xticks(fontsize="x-large")

# To have a graph that can be easily included in another document
plt.tight_layout(pad=0.5)


# filename for the output
if options.output is None:
    prefix, ext = os.path.splitext(options.input)
    outname = prefix + ".png"
else:
    outname = options.output

plt.savefig(outname, format="png", dpi=1200)
