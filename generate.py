import json
import random

# Open the file in write mode
with open("order_book.json", "w") as f:
    # Generate 1000 orders and write each one to a new line
    for _ in range(30):
        order = {
            "Order": {
                "price": random.randint(60, 80),  # Random price between 60 and 80
                "quantity": random.randint(100, 200),  # Random quantity between 100 and 200
                "type": random.choice([0, 1]),  # Random type, either 1 or 2
                "side": random.choice([0, 1])  # Random side, either 0 or 1
            }
        }
        # Write each order on a new line in the file
        f.write(json.dumps(order) + "\n")

print("Order book saved to order_book.json")
