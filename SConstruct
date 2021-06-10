from utils import *
from util import cmloptions, loadoptions, check_if_dir_exist
from init_env import init_environment
import platform
import SCons
import os
from SCons.Environment import *
from SCons.Variables import *
from SCons.Script import *


mc_enable = ARGUMENTS.get('MC',0)
if int(mc_enable):
    print "MC support is enabled"
    env = init_environment("qt5 root geant4 clhep evio xercesc ccdb mlibrary clas12")
    env.AppendUnique(CPPDEFINES='MC_SUPPORT_ENABLE')
else:
    print bcolors.WARNING," no MC support",bcolors.ENDC
    #Need a manual setting
    opts = Variables()
    cmloptions(opts)
    env = Environment(tools=['default'], options = opts, ENV = os.environ)
    from loadccdb import loadccdb
    loadccdb(env)
    from loadclhep import loadclhep
    loadclhep(env)
    from loadevio import loadevio
    loadevio(env)
 #    from loadjana import loadjana
 #   loadjana(env)
    from loadroot import loadroot
    loadroot(env)
    from loadxerces import loadxerces
    loadxerces(env)
    loadoptions(env)




debug = ARGUMENTS.get('debug', 0)
if int(debug):
   print "DEBUG IS ON"
   env.Append(CCFLAGS = '-g')

et_enable = ARGUMENTS.get('ET',0)
if int(et_enable):
    print "ET-ring and ONLINE support is enabled"
    env.AppendUnique(CPPDEFINES='ET_SUPPORT_ENABLE')


#A.C. probably do this better
if (platform.system()=="Darwin"):
	print "We are on MAC"
	env.AppendUnique(LINKFLAGS='-flat_namespace')
	env.AppendUnique(SHLINKFLAGS=['-undefined', 'suppress'])
 
env.Append(CPPPATH=Dir('#/src/external').srcnode().abspath)
env.Append(CPPPATH=Dir('#/src/libraries').srcnode().abspath)
env.Append(CPPPATH=Dir('#/src/plugins').srcnode().abspath)
env.Append(CPPPATH=Dir('#/.').srcnode().abspath)
env.Append(LIBPATH = ['#/lib'])

env.Replace(RPATH=Dir('#/lib').srcnode().abspath)

#This drove me crazy! jlab_software jana was including this, and we need it for sophisticated rootspy features..
env.Append(LINKFLAGS = '-rdynamic')

Export('env debug et_enable mc_enable')

libExt=SConscript('src/external/SConstruct')
lib=SConscript('src/libraries/SConstruct')
progs=SConscript('src/programs/SConstruct')
plugins=SConscript('src/plugins/SConstruct')










