// INTEL RUN: %clangxx -Xclang -opaque-pointers -target x86_64-unknown-unknown -g \
// INTEL RUN:   %s -emit-llvm -S -o - | FileCheck %s

// INTEL RUN: %clangxx -Xclang -opaque-pointers -target x86_64-unknown-unknown -g \
// INTEL RUN:   -fno-elide-constructors %s -emit-llvm -S -o - | \
// INTEL RUN:   FileCheck %s -check-prefix=NOELIDE

struct Foo {
  Foo() = default;
  Foo(Foo &&other) { x = other.x; }
  int x;
};
void some_function(int);
Foo getFoo() {
  Foo foo;
  foo.x = 41;
  some_function(foo.x);
  return foo;
}

int main() {
  Foo bar = getFoo();
  return bar.x;
}

// Check that NRVO variables are stored as a pointer with deref if they are
// stored in the return register.

// CHECK: %[[RESULT:.*]] = alloca ptr, align 8
// CHECK: call void @llvm.dbg.declare(metadata ptr %[[RESULT]],
// CHECK-SAME: metadata !DIExpression(DW_OP_deref)

// NOELIDE: %[[FOO:.*]] = alloca %struct.Foo, align 4
// NOELIDE: call void @llvm.dbg.declare(metadata ptr %[[FOO]],
// NOELIDE-SAME:                        metadata !DIExpression()
