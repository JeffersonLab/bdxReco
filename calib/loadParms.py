                                                                                                                                                                                                                            
# -*- coding: utf-8 -*-                                                                                                                                                                                                                             

import string,math,os,sys,subprocess
import argparse,glob

#check current working dir
if ("BDXRECO_ROOT" in os.environ)==0:
    print "BDXRECO_ROOT enviromental variable not defined. Exit"
    sys.exit()

command = "cd "+os.environ['BDXRECO_ROOT']+"/calib ; pwd "
p = subprocess.Popen(command,shell=True,stdout=subprocess.PIPE)
bdx_fold , err = p.communicate()

command = "pwd"
p = subprocess.Popen(command,shell=True,stdout=subprocess.PIPE)
cur_fold , err = p.communicate()

if (cur_fold != bdx_fold):
    print "Error, cur folder is ",cur_fold
    print "This script is supposed to run from ",bdx_fold
    sys.exit()

#Chek input arguments
#this should be: -v Variation -min runMin -max runMax
#if variation is not specified, use "default"
#if min and/or max are not specified, used CCDB default behavior



parser = argparse.ArgumentParser(description='Load data in CCDB for given variation / runs')
parser.add_argument('--variation',type=str, default="default", help='If set, set the variation to use.')
parser.add_argument('--connection',type=str,required=True,help="Connection string to use")
parser.add_argument('--firstRun',type=int,default=0,help="minimum run to consider in the folder")
parser.add_argument('--lastRun',type=int,default=99999,help="maximum run to consider in the folder")
parser.add_argument('--LSB',type=float,default=0.4884,help="LSB value")
parser.add_argument('--dT',type=float,default=4.,help="SAMPLING TIME")
args = parser.parse_args()

variation = args.variation
connection_string = args.connection
firstRun = args.firstRun
lastRun = args.lastRun
LSB     = args.LSB
dT      = args.dT
#check if DAQ_pedestals folder exists
if (os.path.isdir("DAQ_parms")==False):
    print "Error, folder DAQ_pedestals does not exists"
    sys.exit()     

commandBase = "ccdb -c "+connection_string+" add -v "+variation

print "Loading pedestals for variation ",variation," and run_min: ",firstRun


#Depending on variation:

#In case of default, do nothing

outF=open("DAQ_parms/tables/parms.dat","w")
for crate in range(0,20):
    for slot in range(0,20):
        for channel in range(0,16):
            outF.write(str(crate)+" "+str(slot)+" "+str(channel)+" "+str(LSB)+" "+str(dT)+" 0 0 0 \n")
outF.close()
#at this point, we have the file for run 
#specify in the command
command = commandBase+" -r "+str(firstRun)+"-"+str(lastRun)
#command = commandBase
command += " /DAQ/parms DAQ_parms/tables/parms.dat"
print command
subprocess.Popen(command,shell=True)


