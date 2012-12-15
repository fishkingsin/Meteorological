a=234
for i in *.tif; do
  new=$(printf "head2_%04d.tif" ${a}) #04 pad to length of 4
  mv ${i} ${new}
  let a=a-1
done