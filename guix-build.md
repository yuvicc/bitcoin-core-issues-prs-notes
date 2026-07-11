# Guix Build & Attestation (server build & laptop signing)

## 1. Prerequisites (server)

```bash
sudo apt update && sudo apt install uidmap gnupg curl xz-utils
```

```bash
sudo sysctl -w kernel.apparmor_restrict_unprivileged_userns=0
```

## 2. Install Guix

```bash
sudo systemctl status guix-daemon
```

```bash
. /etc/profile.d/guix.sh
guix --version
```

```bash
guix pull
hash guix
guix --version
```

```bash
guix archive --authorize < ~/.config/guix/current/share/guix/ci.guix.gnu.org.pub
guix archive --authorize < ~/.config/guix/current/share/guix/bordeaux.guix.gnu.org.pub
```

```bash
guix install glibc-locales
```

## 3. Source & repos

```bash
git clone https://github.com/bitcoin/bitcoin.git ~/bitcoin
cd ~/bitcoin
git checkout <version-tag>
git verify-tag <version-tag>
```

```bash
git clone https://github.com/bitcoin-core/guix.sigs.git ~/guix.sigs
git clone https://github.com/bitcoin-core/bitcoin-detached-sigs.git ~/bitcoin-detached-sigs
```

## 4. Build (using tmux)

```bash
tmux new -s guix
```

```bash
cd ~/bitcoin
export JOBS=4
export HOSTS="x86_64-linux-gnu arm-linux-gnueabihf aarch64-linux-gnu riscv64-linux-gnu powerpc64-linux-gnu powerpc64le-linux-gnu x86_64-w64-mingw32"
./contrib/guix/guix-build
```
*Note: non darwin*

## 5. Codesigning

```bash
cd ~/bitcoin-detached-sigs
git fetch --tags
git checkout <version-tag>
```

```bash
cd ~/bitcoin
env DETACHED_SIGS_REPO=~/bitcoin-detached-sigs HOSTS="x86_64-w64-mingw32" ./contrib/guix/guix-codesign
```

## 6. Attest (no signing)

```bash
rm -rf ~/guix.sigs/29.4/yuvicc/
env GUIX_SIGS_REPO=~/guix.sigs SIGNER=yuvicc NO_SIGN=1 ./contrib/guix/guix-attest
```

```bash
ls ~/guix.sigs/<version-tag>/yuvicc/
```

## 7. Verify against other builders (server)

```bash
git -C ~/guix.sigs pull
gpg --import ~/guix.sigs/builder-keys/*.gpg
env GUIX_SIGS_REPO=~/guix.sigs ./contrib/guix/guix-verify
```

## 8. Fork remotes (local machine)

```bash
cd ~/guix.sigs
git remote rename origin upstream
git remote add origin git@github.com:yuvicc/guix.sigs.git
git remote -v
```

```bash
git fetch upstream
git checkout main
git merge upstream/main
```

## 9. Get the branch onto the laptop

```bash
git fetch origin
git checkout <version-tag>
```

## 10. Sign (local machine)

```bash
gpg --list-secret-keys --keyid-format long
gpg --show-keys ~/guix.sigs/builder-keys/yuvicc.gpg
```

```bash
cd ~/guix.sigs/29.4/yuvicc
for f in noncodesigned.SHA256SUMS all.SHA256SUMS; do
  gpg -u <FINGERPRINT> --detach-sign --armor --output "$f.asc" "$f"
done
```

```bash
gpg --verify noncodesigned.SHA256SUMS.asc noncodesigned.SHA256SUMS
gpg --verify all.SHA256SUMS.asc all.SHA256SUMS
```
