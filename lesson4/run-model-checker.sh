# visualize.py -t ( tree ) / -r 可选
python3 ./model-checker.py ./mutex-bad.py  | python3 visualize.py > mutex-bad.html
python3 ./model-checker.py ./dekker.py  | python3 visualize.py -r > dekker.html
python3 ./model-checker.py ./peterson-flag.py  | python3 visualize.py -t > peterson-flag.html
explorer.exe .
