; ModuleID = 'test5.cpp'
source_filename = "test5.cpp"
target datalayout = "e-p:32:32-i32:32"
target triple = "p2"

; Function Attrs: noinline nounwind optnone
define dso_local i32 @_Z3sumiiiii(i32 %a, i32 %b, i32 %c, i32 %d, i32 %e) #0 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  %c.addr = alloca i32, align 4
  %d.addr = alloca i32, align 4
  %e.addr = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  store i32 %b, i32* %b.addr, align 4
  store i32 %c, i32* %c.addr, align 4
  store i32 %d, i32* %d.addr, align 4
  store i32 %e, i32* %e.addr, align 4
  %0 = load i32, i32* %a.addr, align 4
  %1 = load i32, i32* %b.addr, align 4
  %add = add nsw i32 %0, %1
  %2 = load i32, i32* %c.addr, align 4
  %add1 = add nsw i32 %add, %2
  %3 = load i32, i32* %d.addr, align 4
  %add2 = add nsw i32 %add1, %3
  %4 = load i32, i32* %e.addr, align 4
  %add3 = add nsw i32 %add2, %4
  ret i32 %add3
}

; Function Attrs: noinline norecurse nounwind optnone
define dso_local i32 @main() #1 {
entry:
  %retval = alloca i32, align 4
  %d = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  %call = call i32 @_Z3sumiiiii(i32 1, i32 2, i32 3, i32 4, i32 5)
  store i32 %call, i32* %d, align 4
  %0 = load i32, i32* %d, align 4
  ret i32 %0
}

attributes #0 = { noinline nounwind optnone "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { noinline norecurse nounwind optnone "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.0 (https://github.com/ne75/llvm-propeller2.git 4d557406a7897e02a2e784d28335c582d21d0c60)"}
