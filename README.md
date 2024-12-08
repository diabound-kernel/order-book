# Order-Book

Default build:

      bazel build //server:server 
      
With a custom compiler:
      
      CC=<compiler-binary> bazel build //server:server

Run:

      ./bazel-bin/server/server order_book.json
