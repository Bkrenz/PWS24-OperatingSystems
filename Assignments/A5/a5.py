import jinja2
from pathlib import Path
from matplotlib import pyplot

environment = jinja2.Environment(
    block_start_string = '\\BLOCK{',
    block_end_string = '}',
    variable_start_string = '\\VAR{',
    variable_end_string = '}',
    comment_start_string = '\\#{',
    comment_end_string = '}',
    line_statement_prefix = '%%',
    line_comment_prefix = '%#',
    trim_blocks = True,
    autoescape = False,
    loader = jinja2.FileSystemLoader('.')
)
template = environment.get_template('a5_template.tex')
out_file = Path('.') / 'a5.tex'

# Get the list of primes
with open('primes.txt', 'r') as primesFile:
    primesList = []
    for line in primesFile:
        primesList.append(int(line.strip()))
primes = ''
for i in range(int(len(primesList)/6)):
    primes += f'\t{primesList[i]} & {primesList[i+1]} & {primesList[i+2]} & {primesList[i+3]} & {primesList[i+4]} & {primesList[i+5]} \\\\ \n'


# Get Time Data for Section A
with open('results_a.csv', 'r') as results:
    data_a = []
    for row in results:
        row = row.strip().split(',')
        time = float(row[1].strip()) / 1000
        data_a.append(time)
results_a = ''
for i in range(15):
    results_a += f'Test {i} & {data_a[i]} & Test {i+15} & {data_a[i+15]} \\\\ \\hline \n'


# Get Threaded Time Data for section B
consumerTableB = ''
with open('results_b.csv', 'r') as results:
    data_b = {
        2 : [],
        3 : [],
        4 : [],
        8 : []
    }
    testNum = 1
    for row in results:
        row = row.strip().split(',')
        consumerCount = float(row[0].strip())
        consumerTime = float(row[1].strip()) / 1000
        data_b[consumerCount].append(consumerTime)
for i in range(30):
    consumerTableB += f'Test {i+1} & {data_b[2][i]} & {data_b[3][i]} & {data_b[4][i]} & {data_b[8][i]} \\\\ \\hline \n'
    

# Get Threaded Time Data for section C
consumerTableC = ''
with open('results_c.csv', 'r') as results:
    data_c = {
        2 : [],
        3 : [],
        4 : [],
        8 : []
    }
    testNum = 1
    for row in results:
        row = row.strip().split(',')
        consumerCount = float(row[0].strip())
        consumerTime = float(row[1].strip()) / 1000
        data_c[consumerCount].append(consumerTime)
for i in range(30):
    consumerTableC += f'Test {i+1} & {data_c[2][i]} & {data_c[3][i]} & {data_c[4][i]} & {data_c[8][i]} \\\\ \\hline \n'

# Create the Consumers Chart
pyplot.plot(data_a, color='red', label='1 Consumer')
pyplot.plot(data_b[2], color='orange', label='2 Consumers')
pyplot.plot(data_b[3], color='yellow', label='3 Consumers')
pyplot.plot(data_b[4], color='green', label='4 Consumers')
pyplot.plot(data_b[8], color='blue', label='8 Consumers')
pyplot.xlabel('Test #')
pyplot.ylabel('Time (sec)')
pyplot.legend()
pyplot.savefig('Consumers.png')

pyplot.clf()

# Create the Item Buffer Chart
for key in data_b:
    if key == 2:
        pyplot.plot(data_b[key], color='g', label='1-Item Buffer')
    else:
        pyplot.plot(data_b[key], color='g')
for key in data_c:
    if key == 2:
        pyplot.plot(data_c[key], color='blue', label='2-Item Buffer')
    else:
        pyplot.plot(data_c[key], color='blue')
pyplot.xlabel('Test #')
pyplot.ylabel('Time (sec)')
pyplot.legend()
pyplot.savefig('ItemBuffer.png')

# Write the Content out to the file
content = template.render(primes=primes, results_b=consumerTableB, results_c=consumerTableC, results_a=results_a)
out_file.write_text(content)