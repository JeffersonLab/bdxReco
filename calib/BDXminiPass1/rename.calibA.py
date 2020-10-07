import os
import glob
files = glob.glob("????.calibA")
#files=glob.glob("*sipm_gain*")
for mfile in files:
	print(mfile)
	run=mfile.split(".")[0]
	#run=mfile.split(".")[2]
	outname="InnerVeto.sipm_ampl."+run
	os.rename(mfile,outname)

