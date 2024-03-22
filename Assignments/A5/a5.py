import jinja2
from pathlib import Path

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

# Write the Content out to the file
content = template.render(primes = primes)
out_file.write_text(content)