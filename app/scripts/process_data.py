import csv
import time
import sys

from collections import defaultdict

def process_data(filename):
    print(f"Starting to process the file {filename}...")

    user_totals = defaultdict(float)
    successful_count = 0
    total_count = 0

    start_time = time.time()

    try:
        with open(filename, mode='r') as file:
            reader = csv.reader(file)

            next(reader)

            for row in reader:
                total_count += 1

                status = row[5]

                if status == 'SUCCESS':
                    user_id = int(row[2])
                    amount = float(row[3])

                    user_totals[user_id] += amount
                    successful_count += 1
    except FileNotFoundError:
        print(f"Error: file {filename} not found")
        sys.exit(1)

    end_time = time.time()
    duration = end_time - start_time

    print("=" * 40)
    print(f"Processing completed in {duration: .2f} seconds.")
    print(f"Total rows: {total_count}")
    print(f"Successful transactions: {successful_count}")
    print(f"Unique users: {len(user_totals)}")
    print("=" * 40)

    print("Top 5 users by transaction amount:")
    top_users = sorted(user_totals.items(), key=lambda item: item[1], reverse=True)[:5]

    for uid, total in top_users:
        print(f"User {uid}: {total:.2f}")

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python process_data.py <file_name>")
        sys.exit(1)

    input_file = sys.argv[1]
    process_data(input_file)

