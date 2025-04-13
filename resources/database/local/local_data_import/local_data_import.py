import sqlite3
import csv
import os

# Paths
db_path = "../dungeoncrawl_local.db"
csv_path = "local_data_input.csv"

def get_or_create_attribute_id(cursor, attribute_name):
    # Try to fetch the attribute ID
    cursor.execute("SELECT AT_ID FROM attribute WHERE AT_NAME = ?", (attribute_name,))
    result = cursor.fetchone()

    if result:
        return result[0]
    else:
        # Insert new attribute and return new ID
        cursor.execute("INSERT INTO attribute (AT_NAME) VALUES (?)", (attribute_name,))
        return cursor.lastrowid

def process_csv_and_update_db():
    # Make sure file exists
    if not os.path.exists(csv_path):
        print(f"CSV file not found: {csv_path}")
        return

    try:
        # Connect to the SQLite database
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()
        print(f"Connected to database: {db_path}")

        # Open and read the CSV file
        with open(csv_path, newline='', encoding='utf-8') as csvfile:
            reader = csv.reader(csvfile, delimiter=';')
            next(reader)  # Skip header

            for line_num, row in enumerate(reader, start=2):
                if len(row) != 6:
                    print(f"Skipping malformed line {line_num}: {row}")
                    continue

                attr_names = row[:4]
                text_de = row[4].strip()
                text_en = row[5].strip()

                # Resolve or insert attributes
                attr_ids = []
                for name in attr_names:
                    name = name.strip()
                    if not name:
                        attr_ids.append(None)
                    else:
                        attr_id = get_or_create_attribute_id(cursor, name)
                        attr_ids.append(attr_id)

                # Insert into localization table
                try:
                    cursor.execute("""
                        INSERT INTO localization 
                            (LC_AT_1, LC_AT_2, LC_AT_3, LC_AT_4, LC_TEXT_DE, LC_TEXT_EN)
                        VALUES (?, ?, ?, ?, ?, ?)
                    """, (*attr_ids, text_de, text_en))
                except sqlite3.IntegrityError as e:
                    print(f"Skipping duplicate or invalid entry at line {line_num}: {e}")

        # Commit all changes
        conn.commit()
        print("Database update complete.")

    except sqlite3.Error as e:
        print(f"SQLite error: {e}")

    finally:
        if conn:
            conn.close()
            print("Connection closed.")

if __name__ == "__main__":
    process_csv_and_update_db()
