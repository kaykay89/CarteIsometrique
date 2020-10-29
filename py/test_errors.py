import sys
from subprocess import Popen, PIPE

TESTS = [
    (['bin/tp2', '--help'], None, 0),
    (['bin/tp2', '--input-filename', 'data/map.json', '--output-format', 'text'], None, 0),
    (['bin/tp2', '--input-filename', 'data/map.json', '--output-format', 'dot'], None, 0),
    (['bin/tp2', '--input-filename', 'data/map.json', '--output-format', 'jpeg'], 'Error: format jpeg not supported', 1),
    (['bin/tp2', '--start', 'a,b,c', '--input-filename', 'data/map.json', '--output-format', 'png'], 'Error: the coordinates must be integers separated by commas', 2),
    (['bin/tp2', '--start', '1,0,0', '--end', '0,1,a', '--input-filename', 'data/map.json', '--output-format', 'png'], 'Error: the coordinates must be integers separated by commas', 2),
    (['bin/tp2', '--input-filename', 'data/map.json', '--output-format', 'png'], 'Error: output filename is mandatory with png format', 3),
    (['bin/tp2', '--input-filename', 'data/map.json', '--strat'], None, 4),
    (['bin/tp2', '--output-format jpeg'], 'Error: input filename is mandatory', 5),
    (['bin/tp2', '--start', '3,0,0', '--end', '9,9,0', '--input-filename', 'data/map.json', '--output-format', 'png', '--output-filename', 'map.png'], 'Error: the cell (9,9,0) does not belong to the map.', 2),
    (['bin/tp2', '--start', '0,9,12', '--end', '0,9,1', '--input-filename', 'data/map.json', '--output-format', 'png', '--output-filename', 'map.png'], 'Error: the cell (0,9,12) does not belong to the map.', 2),
    (['bin/tp2', '--start', '1,0,0', '--end', '1,2,2', '--input-filename', 'data/map3x3error.json', '--output-format', 'png', '--output-filename', 'map3x3.png'], 'Error: Invalid JSON file', 6),
    (['bin/tp2', '--start', '1,0,0', '--end', '1,2,2', '--input-filename', 'data/map3x3iderror.json', '--output-format', 'png', '--output-filename', 'map3x3.png'], 'Error: Invalid JSON file', 6),
]

print '-----------------------'
print 'Testing the program tp2'
print '-----------------------\n'

n = len(TESTS)
i = 1
p = 0
for command, expected_output, expected_code in TESTS:
    print 'Test %s of %s...' % (i, n),
    process = Popen(command, stdout=PIPE, stderr=PIPE)
    output, _ = process.communicate()
    code = process.returncode
    output = output.strip()
    if expected_output is not None:
        expected_output = expected_output.strip()
    if (expected_output is None or output == expected_output)\
            and code == expected_code:
        print 'passed'
        p += 1
    else:
        print 'failed'
        print 'command:'
        print ' '.join(command)
        print 'output:'
        print output
        print 'expected:'
        print expected_output
        print 'returned code:'
        print code
        print 'expected returned code:'
        print expected_code
    print
    i += 1

print '----------------'
print 'Result: %s / %s' % (p, n)
print '----------------'

if p != n:
    sys.exit('Some tests failed!!!')
