for c in 50 100 200 400 800; do
  echo "=== c=$c ==="
  wrk -t4 -c$c -d20s http://127.0.0.1:5005/
done