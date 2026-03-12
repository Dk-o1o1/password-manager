import secrets
import string
import json
import hashlib
import getpass
import os
from datetime import datetime

FILE_NAME = "passwords.json"

# ==================== HELPER FUNCTIONS ====================
def hash_password(password):
    return hashlib.sha256(password.encode()).hexdigest()

def load_data():
    if os.path.exists(FILE_NAME):
        with open(FILE_NAME, "r") as f:
            return json.load(f)
    return {"master_hash": "", "accounts": {}}

def save_data(data):
    with open(FILE_NAME, "w") as f:
        json.dump(data, f, indent=4)

def generate_password(length=16, use_symbols=True, use_numbers=True):
    letters = string.ascii_letters
    digits = string.digits
    symbols = "!@#$%^&*()_+=[]{}|;:,.<>?"
    
    characters = letters
    if use_numbers:
        characters += digits
    if use_symbols:
        characters += symbols
    
    password = ''.join(secrets.choice(characters) for _ in range(length))
    return password

def check_strength(password):
    score = 0
    if len(password) >= 12: score += 1
    if any(c.isupper() for c in password): score += 1
    if any(c.islower() for c in password): score += 1
    if any(c.isdigit() for c in password): score += 1
    if any(c in "!@#$%^&*()_+=[]{}|;:,.<>?" for c in password): score += 1
    
    if score >= 5: return "🔥 STRONG"
    elif score >= 3: return "🟡 MEDIUM"
    else: return "🔴 WEAK"

# ==================== MAIN PROGRAM ====================
print("🔐 Welcome to Advanced PyPassword Manager v2.0 🔥\n")

data = load_data()

# First-time master password setup
if not data["master_hash"]:
    print("=== FIRST TIME SETUP ===")
    while True:
        master = getpass.getpass("Set a New Master Password: ")
        confirm = getpass.getpass("Confirm it: ")
        if master == confirm:
            data["master_hash"] = hash_password(master)
            save_data(data)
            print("✅ Master Password successfully set!\n")
            break
        else:
            print("❌ Passwords do not match. Please try again.\n")

# Session authentication
authenticated = False
while not authenticated:
    master_input = getpass.getpass("🔑 Enter Master Password to continue: ")
    if hash_password(master_input) == data["master_hash"]:
        authenticated = True
        print("✅ Access Granted! Welcome back!\n")
    else:
        print("❌ Incorrect Master Password! Please try again.\n")

# Main Menu Loop
while True:
    print("\n" + "="*50)
    print("                MAIN MENU")
    print("="*50)
    print("1. Generate New Password")
    print("2. View All Saved Passwords")
    print("3. Delete a Password")
    print("4. Exit")
    print("="*50)
    
    choice = input("\nWhat would you like to do? (1/2/3/4): ").strip()
    
    if choice == "1":
        print("\n--- New Password Generator ---")
        length = int(input("Password length? (Recommended: 16): ") or 16)
        use_sym = input("Include symbols? (y/n): ").lower() == "y"
        use_num = input("Include numbers? (y/n): ").lower() == "y"
        
        password = generate_password(length, use_sym, use_num)
        strength = check_strength(password)
        
        print(f"\n✅ Generated Password: {password}")
        print(f"   Strength: {strength}")
        
        save_it = input("\nDo you want to save this password? (yes/no): ").lower()
        if save_it in ["yes", "y"]:
            account = input("Account name (e.g., Gmail, Instagram, etc.): ").strip()
            data["accounts"][account] = {
                "password": password,
                "strength": strength,
                "created": datetime.now().strftime("%Y-%m-%d %H:%M")
            }
            save_data(data)
            print(f"✅ Saved successfully for {account}!")
    
    elif choice == "2":
        if not data["accounts"]:
            print("No passwords saved yet.")
        else:
            print(f"\n{'Account':<20} {'Password':<25} {'Strength':<10} {'Date'}")
            print("-" * 70)
            for acc, info in data["accounts"].items():
                print(f"{acc:<20} {info['password']:<25} {info['strength']:<10} {info['created']}")
    
    elif choice == "3":
        if not data["accounts"]:
            print("No passwords available to delete.")
        else:
            print("\nSaved Accounts:")
            for i, acc in enumerate(data["accounts"].keys(), 1):
                print(f"{i}. {acc}")
            try:
                idx = int(input("\nWhich account number to delete?: ")) - 1
                account_to_del = list(data["accounts"].keys())[idx]
                confirm = input(f"Are you sure you want to delete {account_to_del}? (yes/no): ").lower()
                if confirm in ["yes", "y"]:
                    del data["accounts"][account_to_del]
                    save_data(data)
                    print("🗑️  Deleted successfully!")
            except:
                print("Invalid input!")
    
    elif choice == "4":
        print("\nThank you for using Advanced PyPassword Manager! 👋")
        break
    
    else:
        print("Invalid choice! Please select from 1-4.")
