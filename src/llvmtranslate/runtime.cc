#include <string>
#include <llvmtranslate/libllvmtranslate.hh>

namespace llvmtranslate
{
  const char* runtime_string()
  {
    return
      R"0(; ModuleID = 'src/llvmtranslate/tiger-runtime.c'
source_filename = "src/llvmtranslate/tiger-runtime.c"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-i128:128-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

@.str = private unnamed_addr constant [29 x i8] c"chr: character out of range\0A\00", align 1
@stderr = external global ptr, align 4
@consts = internal global [512 x i8] zeroinitializer, align 1
@.str.1 = private unnamed_addr constant [36 x i8] c"substring: arguments out of bounds\0A\00", align 1
@stdin = external global ptr, align 4
@.str.2 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@.str.3 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str.4 = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@stdout = external global ptr, align 4

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local ptr @tc_init_array(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca ptr, align 4
  %6 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  store i32 %1, ptr %4, align 4
  %7 = load i32, ptr %3, align 4
  %8 = mul i32 %7, 4
  %9 = call noalias ptr @malloc(i32 noundef %8) #6
  store ptr %9, ptr %5, align 4
  store i32 0, ptr %6, align 4
  br label %10

10:                                               ; preds = %19, %2
  %11 = load i32, ptr %6, align 4
  %12 = load i32, ptr %3, align 4
  %13 = icmp ult i32 %11, %12
  br i1 %13, label %14, label %22

14:                                               ; preds = %10
  %15 = load i32, ptr %4, align 4
  %16 = load ptr, ptr %5, align 4
  %17 = load i32, ptr %6, align 4
  %18 = getelementptr inbounds i32, ptr %16, i32 %17
  store i32 %15, ptr %18, align 4
  br label %19

19:                                               ; preds = %14
  %20 = load i32, ptr %6, align 4
  %21 = add i32 %20, 1
  store i32 %21, ptr %6, align 4
  br label %10

22:                                               ; preds = %10
  %23 = load ptr, ptr %5, align 4
  ret ptr %23
}

; Function Attrs: nounwind allocsize(0)
declare noalias ptr @malloc(i32 noundef) #1

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local i32 @tc_not(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %4 = icmp ne i32 %3, 0
  %5 = xor i1 %4, true
  %6 = zext i1 %5 to i32
  ret i32 %6
}

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local void @tc_exit(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  call void @exit(i32 noundef %3) #7
  unreachable
}

; Function Attrs: noreturn nounwind
declare void @exit(i32 noundef) #2

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local ptr @tc_chr(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %4 = icmp sle i32 0, %3
  br i1 %4, label %5, label %8

5:                                                ; preds = %1
  %6 = load i32, ptr %2, align 4
  %7 = icmp sle i32 %6, 255
  br i1 %7, label %11, label %8

8:                                                ; preds = %5, %1
  %9 = load ptr, ptr @stderr, align 4
  %10 = call i32 @fputs(ptr noundef @.str, ptr noundef %9)
  call void @exit(i32 noundef 120) #7
  unreachable

11:                                               ; preds = %5
  %12 = load i32, ptr %2, align 4
  %13 = mul nsw i32 %12, 2
  %14 = getelementptr inbounds i8, ptr @consts, i32 %13
  ret ptr %14
}

declare i32 @fputs(ptr noundef, ptr noundef) #3

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local ptr @tc_concat(ptr noundef %0, ptr noundef %1) #0 {
  %3 = alloca ptr, align 4
  %4 = alloca ptr, align 4
  %5 = alloca ptr, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca ptr, align 4
  store ptr %0, ptr %4, align 4
  store ptr %1, ptr %5, align 4
  %11 = load ptr, ptr %4, align 4
  %12 = call i32 @strlen(ptr noundef %11) #8
  store i32 %12, ptr %6, align 4
  %13 = load ptr, ptr %5, align 4
  %14 = call i32 @strlen(ptr noundef %13) #8
  store i32 %14, ptr %7, align 4
  %15 = load i32, ptr %6, align 4
  %16 = icmp eq i32 %15, 0
  br i1 %16, label %17, label %19

17:                                               ; preds = %2
  %18 = load ptr, ptr %5, align 4
  store ptr %18, ptr %3, align 4
  br label %69

19:                                               ; preds = %2
  %20 = load i32, ptr %7, align 4
  %21 = icmp eq i32 %20, 0
  br i1 %21, label %22, label %24

22:                                               ; preds = %19
  %23 = load ptr, ptr %4, align 4
  store ptr %23, ptr %3, align 4
  br label %69

24:                                               ; preds = %19
  store i32 0, ptr %8, align 4
  %25 = load i32, ptr %6, align 4
  %26 = load i32, ptr %7, align 4
  %27 = add i32 %25, %26
  store i32 %27, ptr %9, align 4
  %28 = load i32, ptr %9, align 4
  %29 = add nsw i32 %28, 1
  %30 = call noalias ptr @malloc(i32 noundef %29) #6
  store ptr %30, ptr %10, align 4
  store i32 0, ptr %8, align 4
  br label %31

31:                                               ; preds = %43, %24
  %32 = load i32, ptr %8, align 4
  %33 = load i32, ptr %6, align 4
  %34 = icmp ult i32 %32, %33
  br i1 %34, label %35, label %46

35:                                               ; preds = %31
  %36 = load ptr, ptr %4, align 4
  %37 = load i32, ptr %8, align 4
  %38 = getelementptr inbounds i8, ptr %36, i32 %37
  %39 = load i8, ptr %38, align 1
  %40 = load ptr, ptr %10, align 4
  %41 = load i32, ptr %8, align 4
  %42 = getelementptr inbounds i8, ptr %40, i32 %41
  store i8 %39, ptr %42, align 1
  br label %43

43:                                               ; preds = %35
  %44 = load i32, ptr %8, align 4
  %45 = add nsw i32 %44, 1
  store i32 %45, ptr %8, align 4
  br label %31

46:                                               ; preds = %31
  store i32 0, ptr %8, align 4
  br label %47

47:                                               ; preds = %61, %46
  %48 = load i32, ptr %8, align 4
  %49 = load i32, ptr %7, align 4
  %50 = icmp ult i32 %48, %49
  br i1 %50, label %51, label %64

51:                                               ; preds = %47
  %52 = load ptr, ptr %5, align 4
  %53 = load i32, ptr %8, align 4
  %54 = getelementptr inbounds i8, ptr %52, i32 %53
  %55 = load i8, ptr %54, align 1
  %56 = load ptr, ptr %10, align 4
  %57 = load i32, ptr %8, align 4
  %58 = load i32, ptr %6, align 4
  %59 = add i32 %57, %58
  %60 = getelementptr inbounds i8, ptr %56, i32 %59
  store i8 %55, ptr %60, align 1
  br label %61

61:                                               ; preds = %51
  %62 = load i32, ptr %8, align 4
  %63 = add nsw i32 %62, 1
  store i32 %63, ptr %8, align 4
  br label %47

64:                                               ; preds = %47
  %65 = load ptr, ptr %10, align 4
  %66 = load i32, ptr %9, align 4
  %67 = getelementptr inbounds i8, ptr %65, i32 %66
  store i8 0, ptr %67, align 1
  %68 = load ptr, ptr %10, align 4
  store ptr %68, ptr %3, align 4
  br label %69

69:                                               ; preds = %64, %22, %17
  %70 = load ptr, ptr %3, align 4
  ret ptr %70
}

; Function Attrs: nounwind willreturn memory(read)
declare i32 @strlen(ptr noundef) #4

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local i32 @tc_ord(ptr noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca ptr, align 4
  %4 = alloca i32, align 4
  store ptr %0, ptr %3, align 4
  %5 = load ptr, ptr %3, align 4
  %6 = call i32 @strlen(ptr noundef %5) #8
  store i32 %6, ptr %4, align 4
  %7 = load i32, ptr %4, align 4
  %8 = icmp eq i32 %7, 0
  br i1 %8, label %9, label %10

9:                                                ; preds = %1
  store i32 -1, ptr %2, align 4
  br label %15

10:                                               ; preds = %1
  %11 = load ptr, ptr %3, align 4
  %12 = getelementptr inbounds i8, ptr %11, i32 0
  %13 = load i8, ptr %12, align 1
  %14 = sext i8 %13 to i32
  store i32 %14, ptr %2, align 4
  br label %15

15:                                               ; preds = %10, %9
  %16 = load i32, ptr %2, align 4
  ret i32 %16
}

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local i32 @tc_size(ptr noundef %0) #0 {
  %2 = alloca ptr, align 4
  store ptr %0, ptr %2, align 4
  %3 = load ptr, ptr %2, align 4
  %4 = call i32 @strlen(ptr noundef %3) #8
  ret i32 %4
}

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local ptr @tc_substring(ptr noundef %0, i32 noundef %1, i32 noundef %2) #0 {
  %4 = alloca ptr, align 4
  %5 = alloca ptr, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca ptr, align 4
  %10 = alloca i32, align 4
  store ptr %0, ptr %5, align 4
  store i32 %1, ptr %6, align 4
  store i32 %2, ptr %7, align 4
  %11 = load ptr, ptr %5, align 4
  %12 = call i32 @strlen(ptr noundef %11) #8
  store i32 %12, ptr %8, align 4
  %13 = load i32, ptr %6, align 4
  %14 = icmp sle i32 0, %13
  br i1 %14, label %15, label %24

15:                                               ; preds = %3
  %16 = load i32, ptr %7, align 4
  %17 = icmp sle i32 0, %16
  br i1 %17, label %18, label %24

18:                                               ; preds = %15
  %19 = load i32, ptr %6, align 4
  %20 = load i32, ptr %7, align 4
  %21 = add nsw i32 %19, %20
  %22 = load i32, ptr %8, align 4
  %23 = icmp ule i32 %21, %22
  br i1 %23, label %27, label %24

24:                                               ; preds = %18, %15, %3
  %25 = load ptr, ptr @stderr, align 4
  %26 = call i32 @fputs(ptr noundef @.str.1, ptr noundef %25)
  call void @exit(i32 noundef 120) #7
  unreachable

27:                                               ; preds = %18
  %28 = load i32, ptr %7, align 4
  %29 = icmp eq i32 %28, 1
  br i1 %29, label %30, label %38

30:                                               ; preds = %27
  %31 = load ptr, ptr %5, align 4
  %32 = load i32, ptr %6, align 4
  %33 = getelementptr inbounds i8, ptr %31, i32 %32
  %34 = load i8, ptr %33, align 1
  %35 = sext i8 %34 to i32
  %36 = mul nsw i32 %35, 2
  %37 = getelementptr inbounds i8, ptr @consts, i32 %36
  store ptr %37, ptr %4, align 4
  br label %64

38:                                               ; preds = %27
  %39 = load i32, ptr %7, align 4
  %40 = add nsw i32 %39, 1
  %41 = call noalias ptr @malloc(i32 noundef %40) #6
  store ptr %41, ptr %9, align 4
  store i32 0, ptr %10, align 4
  br label %42

42:                                               ; preds = %56, %38
  %43 = load i32, ptr %10, align 4
  %44 = load i32, ptr %7, align 4
  %45 = icmp slt i32 %43, %44
  br i1 %45, label %46, label %59

46:                                               ; preds = %42
  %47 = load ptr, ptr %5, align 4
  %48 = load i32, ptr %6, align 4
  %49 = load i32, ptr %10, align 4
  %50 = add nsw i32 %48, %49
  %51 = getelementptr inbounds i8, ptr %47, i32 %50
  %52 = load i8, ptr %51, align 1
  %53 = load ptr, ptr %9, align 4
  %54 = load i32, ptr %10, align 4
  %55 = getelementptr inbounds i8, ptr %53, i32 %54
  store i8 %52, ptr %55, align 1
  br label %56

56:                                               ; preds = %46
  %57 = load i32, ptr %10, align 4
  %58 = add nsw i32 %57, 1
  store i32 %58, ptr %10, align 4
  br label %42

59:                                               ; preds = %42
  %60 = load ptr, ptr %9, align 4
  %61 = load i32, ptr %7, align 4
  %62 = getelementptr inbounds i8, ptr %60, i32 %61
  store i8 0, ptr %62, align 1
  %63 = load ptr, ptr %9, align 4
  store ptr %63, ptr %4, align 4
  br label %64

64:                                               ; preds = %59, %30
  %65 = load ptr, ptr %4, align 4
  ret ptr %65
}

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local i32 @tc_strcmp(ptr noundef %0, ptr noundef %1) #0 {
  %3 = alloca ptr, align 4
  %4 = alloca ptr, align 4
  store ptr %0, ptr %3, align 4
  store ptr %1, ptr %4, align 4
  %5 = load ptr, ptr %3, align 4
  %6 = load ptr, ptr %4, align 4
  %7 = call i32 @strcmp(ptr noundef %5, ptr noundef %6) #8
  ret i32 %7
}

; Function Attrs: nounwind willreturn memory(read)
declare i32 @strcmp(ptr noundef, ptr noundef) #4

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local i32 @tc_streq(ptr noundef %0, ptr noundef %1) #0 {
  %3 = alloca ptr, align 4
  %4 = alloca ptr, align 4
  store ptr %0, ptr %3, align 4
  store ptr %1, ptr %4, align 4
  %5 = load ptr, ptr %3, align 4
  %6 = load ptr, ptr %4, align 4
  %7 = call i32 @strcmp(ptr noundef %5, ptr noundef %6) #8
  %8 = icmp eq i32 %7, 0
  %9 = zext i1 %8 to i32
  ret i32 %9
}

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local ptr @tc_getchar() #0 {
  %1 = alloca ptr, align 4
  %2 = alloca i32, align 4
  %3 = load ptr, ptr @stdin, align 4
  %4 = call i32 @getc(ptr noundef %3)
  store i32 %4, ptr %2, align 4
  %5 = load i32, ptr %2, align 4
  %6 = icmp eq i32 %5, -1
  br i1 %6, label %7, label %8

7:                                                ; preds = %0
  store ptr @.str.2, ptr %1, align 4
  br label %12

8:                                                ; preds = %0
  %9 = load i32, ptr %2, align 4
  %10 = mul nsw i32 %9, 2
  %11 = getelementptr inbounds i8, ptr @consts, i32 %10
  store ptr %11, ptr %1, align 4
  br label %12

12:                                               ; preds = %8, %7
  %13 = load ptr, ptr %1, align 4
  ret ptr %13
}

declare i32 @getc(ptr noundef) #3

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local void @tc_print(ptr noundef %0) #0 {
  %2 = alloca ptr, align 4
  store ptr %0, ptr %2, align 4
  %3 = load ptr, ptr %2, align 4
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str.3, ptr noundef %3)
  ret void
}

declare i32 @printf(ptr noundef, ...) #3

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local void @tc_print_err(ptr noundef %0) #0 {
  %2 = alloca ptr, align 4
  store ptr %0, ptr %2, align 4
  %3 = load ptr, ptr @stderr, align 4
  %4 = load ptr, ptr %2, align 4
  %5 = call i32 (ptr, ptr, ...) @fprintf(ptr noundef %3, ptr noundef @.str.3, ptr noundef %4) #9
  ret void
}

; Function Attrs: nounwind
declare i32 @fprintf(ptr noundef, ptr noundef, ...) #5

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local void @tc_print_int(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str.4, i32 noundef %3)
  ret void
}

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local void @tc_flush() #0 {
  %1 = load ptr, ptr @stdout, align 4
  %2 = call i32 @fflush(ptr noundef %1)
  ret void
}

declare i32 @fflush(ptr noundef) #3

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  store i32 0, ptr %2, align 4
  br label %3

3:                                                ; preds = %15, %0
  %4 = load i32, ptr %2, align 4
  %5 = icmp slt i32 %4, 512
  br i1 %5, label %6, label %18

6:                                                ; preds = %3
  %7 = load i32, ptr %2, align 4
  %8 = sdiv i32 %7, 2
  %9 = trunc i32 %8 to i8
  %10 = load i32, ptr %2, align 4
  %11 = getelementptr inbounds [512 x i8], ptr @consts, i32 0, i32 %10
  store i8 %9, ptr %11, align 1
  %12 = load i32, ptr %2, align 4
  %13 = add nsw i32 %12, 1
  %14 = getelementptr inbounds [512 x i8], ptr @consts, i32 0, i32 %13
  store i8 0, ptr %14, align 1
  br label %15

15:                                               ; preds = %6
  %16 = load i32, ptr %2, align 4
  %17 = add nsw i32 %16, 2
  store i32 %17, ptr %2, align 4
  br label %3

18:                                               ; preds = %3
  call void @tc_main(i32 noundef 0)
  ret i32 0
}

declare void @tc_main(i32 noundef) #3

attributes #0 = { noinline nounwind optnone sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nounwind allocsize(0) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { noreturn nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nounwind willreturn memory(read) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { nounwind allocsize(0) }
attributes #7 = { noreturn nounwind }
attributes #8 = { nounwind willreturn memory(read) }
attributes #9 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3, !4, !5}
!llvm.ident = !{!6}

!0 = !{i32 1, !"NumRegisterParameters", i32 0}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 8, !"PIC Level", i32 2}
!3 = !{i32 7, !"PIE Level", i32 2}
!4 = !{i32 7, !"uwtable", i32 2}
!5 = !{i32 7, !"frame-pointer", i32 2}
!6 = !{!"clang version 19.1.7"}
)0";
  }
} // namespace llvmtranslate
