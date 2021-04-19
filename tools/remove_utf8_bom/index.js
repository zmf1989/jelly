﻿var fs = require('fs');
var options = {encoding:"utf8"};

var filename = process.argv.length > 2 ? process.argv[2] : "test.c";

var content = fs.readFileSync(filename).toString();
if(content.charCodeAt(0) === 0xfeff) {
  content = content.substr(1);
  fs.writeFileSync(filename, content, options);
  console.log(filename + " done");
}

