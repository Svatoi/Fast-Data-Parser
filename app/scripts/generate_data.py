import csv
import random
import uuid
import time
import sys

def generate_transactions(filename, num_rows):
    currencies = ['USD', 'EUR', 'UAH', 'JPY']
    statuses = ['SUCCESS', 'FAILED', 'PENDING']

    status_weights = [0.7, 0.2, 0.1]

    start_time = time.time()

    print(f"Generating {num_rows} lines in file {filename}...")

    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['transaction_id','  timestamp', '  user_id', '  amount', '  currency', '  status\n'])

        batch_size = 100000
        batch = []

        for i in range(1, num_rows + 1):
            tx_id = str(uuid.uuid4())
            ts = int(time.time()) - random.randint(0, 31536000)
            user_id = random.randint(1, 10000)
            amount = round(random.uniform(5.0, 5000.0), 2)
            currency = random.choice(currencies)
            status = random.choices(statuses, weights=status_weights)[0]

            batch.append([tx_id, ts, user_id, amount, currency, status])

            if i % batch_size == 0:
                writer.writerows(batch)
                batch.clear()
                print(f"Generated {i} / {num_rows}...")

        if batch:
            writer.writerows(batch)

    end_time = time.time()
    print(f"Done! File {filename} created in a couple of {end_time - start_time:.2f} seconds ")

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python generate_data.py <number_of_lines> <filename>")
        # Example -> [ python scripts/generate_data.py 10000000 data.csv ]
        sys.exit(1)

    try:
        rows = int(sys.argv[1])
        out_file = sys.argv[2]
        generate_transactions(out_file, rows)
    except ValueError:
        print("Error: the number of lines must be a number")