import json
import numpy


def load_data():
    data = {}
    data[1] = []
    data[2] = []
    data[3] = []
    data[4] = []
    data[8] = []

    with open('results.csv', 'r') as results:
        for row in results:
            row = row.strip().split(',')
            threadCount = int(row[0])
            totalTime = int(row[1])
            result = {}
            result['time'] = totalTime
            result['threads'] = []
            for i in range(threadCount):
                threadResult = {'id': i+1, 'primesFound': row[i*2+2], 'time': row[i*2+3]}
                result['threads'].append(threadResult)
            data[threadCount].append(result)
    
    with open('result.json', 'w') as jf:
        jf.write(json.dumps(data, indent=4))
    return data


'''
table layout:

        | total time  | thread 1 | ...
test #  | time(ms)    | time(ms) | ...
'''
def generate_thread_table(threadCount: int, data: dict):
    with open(f'data/{threadCount}-threads.txt', 'w') as file:
        # Create the beginning calue for the tabular table
        tabular_begin = '\\begin{tabular}{ | c | c | '
        for i in range(threadCount):
            tabular_begin += 'c | '
        tabular_begin += '}\n\\hline'
        file.write(tabular_begin)

        # Create the Column Headers
        col_headers = '\t & Total '
        for i in range(threadCount):
            col_headers += f'& T{i+1} '
        col_headers += '\\\\ \\hline \n'
        file.write(col_headers)

        # Write Each Test Row
        testNum = 1
        for test in data:
            test_row = f'Test {testNum}'
            testNum += 1
            time = int(test['time']) / 1000
            test_row += f'& {time:.1f} s'
            for thread in test['threads']:
                threadTime = int(thread['time']) / 1000
                test_row += f'& {threadTime:.1f} s'
            test_row += '\\\\ \\hline \n'
            file.write(test_row)

        # Create the end clause for the tabular table
        file.write('\end{tabular}\n')


def calc_stats(data: dict):
    averages = {}
    for threadCount in data:
        results = data[threadCount]
        times = [int(x['time']) for x in results]
        avg = numpy.mean(times) / 1000
        averages[threadCount] = {'avg': avg}
    for threadCount in averages:
        one_thread_avg = averages[1]['avg']
        cur_threads_avg = averages[threadCount]['avg']
        averages[threadCount]['rel'] = f'{cur_threads_avg / one_thread_avg * 100 : .2f}%'
    print(json.dumps(averages, indent=4))



def generate_thread_tables(data: dict):
    for i in data:
        table_data = data[i]
        generate_thread_table(i, table_data)


if __name__ == '__main__':
    data = load_data()
    generate_thread_tables(data)
    calc_stats(data)