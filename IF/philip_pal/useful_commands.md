Testing a package locally
```
sudo pip3 uninstall -y philip_pal
rm -rf dist
python3 setup.py sdist
sudo pip3 install dist/philip_pal-x.x.x.tar.gz
```

Upload to pip
```
twine upload dist/*
```

Basic test
```
python3 setup.py test
```

Reset regression test
```
python3 setup.py test --addopts --regtest-reset
```

View regression output
```
python3 setup.py test --addopts --regtest-tee
```
