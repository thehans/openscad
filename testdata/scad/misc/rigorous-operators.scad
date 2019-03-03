// Tests all combinations of value types with all operators
values = [
  undef, false, true
  -1, 0, 1, 1/0, -1/0, 0/0,
  "", "A", "Z", "aa", "zz",
  [], [-1], [1], [1,2,3],
  [0:10], [-10:0],
];

function fmt(x) = is_string(x) ?  str("\"",x,"\"") : x;

echo("NOTE: Import as tab-separated values into spreadsheet app for easier formatting");

for (x = values) {
  echo(str(
    "x=", fmt(x),
    "\t-x=", fmt(-x),
    "\t+x=", fmt(+x),
    "\t!x=", fmt(!x),
    "\t"
  ));
}

for (x = values) {
  for (y = values) {
    echo(str(
      "\tx=",fmt(x),", y=",fmt(y),
      "\t(x==y)=", fmt(x==y),
      "\t(x!=y)=", fmt(x!=y),
      "\t(x<=y)=", fmt(x<=y),
      "\t(x>=y)=", fmt(x>=y),
      "\t(x&&y)=", fmt(x&&y),
      "\t(x||y)=", fmt(x||y),
      "\t(x<y)=", fmt(x<y),
      "\t(x>y)=", fmt(x>y),
      "\t(x+y)=", fmt(x+y),
      "\t(x-y)=", fmt(x-y),
      "\t(x*y)=", fmt(x*y),
      "\t(x/y)=", fmt(x/y),
      "\t(x%y)=", fmt(x%y),
      "\t"
    ));
  }
}
