import pysodium

passwd = "Test"

pw = "Test"

out = pysodium.crypto_pwhash_str(passwd, pysodium.crypto_pwhash_argon2id_OPSLIMIT_SENSITIVE, pysodium.crypto_pwhash_argon2id_MEMLIMIT_SENSITIVE)

print("Passwords Match?: " + str(pysodium.crypto_pwhash_str_verify(out,pw)))


print(out)
