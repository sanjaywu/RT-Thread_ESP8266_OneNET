from building import *

cwd     = GetCurrentDir()
src     = Glob('*.c')
path    = [cwd]

group = DefineGroup('bh1750', src, depend = ['PKG_USING_BH1750_V100'], CPPPATH = path)

Return('group')
