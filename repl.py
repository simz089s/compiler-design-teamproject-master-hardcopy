import os, sys, subprocess
try:
    input = raw_input
except NameError:
    pass

TEMP_FILE = 'repl_prog.tmp'
# GOLITEC = input('Full (or relative to project root) path to golitec:\n')
GOLITEC = '../golitec'

print("Input 'exit' or 'quit' to close.")


with open(TEMP_FILE,"w+") as f:
    f.write('package repl\n')
    f.flush()
    while True:
        line = input('> ')
        if line in ('quit', 'exit'):
            break
        if line in ('r', 'restart'):
            f.read()
            f.seek(0)
            f.truncate(0)
            f.write('package repl\n')
        else:
            while line[-1] == ' ':
                line += '\n' + input('> ')
            f.write('%s\n' % (line))
        f.flush()
        result = os.system("%s typecheck < %s" % (GOLITEC, TEMP_FILE))
        if result != 0:
            f.read()
            f.seek(0)
            f.truncate(0)
            f.write('package repl\n')
            f.flush()
            print('RESTARTED')

print('DONE.')
os.remove(TEMP_FILE)
