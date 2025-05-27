# SConscript file for rt-thread
import os
import rtconfig
from building import *

cwd     = GetCurrentDir()
src     = [src/xyc_als21c_k1.cpp, othersrc/xyc_als21c_k1_rtthread.c ]
path    = [cwd + "/src"]

LOCAL_CCFLAGS = '-x c'

group = DefineGroup('xyc-asl21c-k1', src, depend = ['PKG_USING_XYC_ALS21C_K1'], CPPPATH = path, LOCAL_CCFLAGS = LOCAL_CCFLAGS)

Return('group')
