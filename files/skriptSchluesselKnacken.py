import subprocess

BASE64_CHARS ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

base64_data = """
SQY8HCkwyhR3ioeo0IpV76Fd3toVYiviCL67RpmFQlgufl...
...gwxc24N6GevNpB1ibwJqvLRnTjUq2smuhyWrAMI9yAZ/Psc=
"""

key_file_path = "corrected_key.pem"

def try_decrypt_rsa_with_openssl(file_path, pin):
    try:
        result = subprocess.run(
            ["openssl", "rsa", "-in", file_path, "-passin", f"pass:{pin}", "-check"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )

        if result.returncode == 0:
            return True
    except Exception:
        pass
    return False

def replace_last_char_and_test(base64_data, key_file_path):

    base64_data = base64_data.replace("\n", "").replace(" ", "")
    base64_data = "\n".join([base64_data[i:i+64] for i in range(0, len(base64_data), 64)])

    for char in BASE64_CHARS:
        print(f"Teste mit Base64-Zeichen: {char}")

        corrected_data = base64_data.replace("=", char)

        with open(key_file_path, 'w') as key_file:
            key_file.write("-----BEGIN RSA PRIVATE KEY-----\n")
            key_file.write("Proc-Type: 4,ENCRYPTED\n")
            key_file.write("DEK-Info: AES-128-CBC,DB7C6A8FA965D19F004829A8CA226678\n\n")
            key_file.write(corrected_data)
            key_file.write("\n-----END RSA PRIVATE KEY-----\n")

        for pin in range(10000):
            pin_str = f"{pin:04d}"
            print(f"Teste PIN: {pin_str}")
            if try_decrypt_rsa_with_openssl(key_file_path, pin_str):
                print(f"Erfolgreich entschl\"usselt mit Base64-Zeichen {char} und PIN {pin_str}")
                return True
    return False

if not replace_last_char_and_test(base64_data, key_file_path):
    print("Kein g\"ultiger PIN oder Base64-Zeichen gefunden.")