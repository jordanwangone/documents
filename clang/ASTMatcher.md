
匹配其左侧恰好是文字0的加法表达式。它不会与其他形式的0（例如'\0'或NULL匹配，但会匹配扩展为0的宏。
匹配器也不会与重载运算符'+'相匹配，有单独的operatorCallExpr匹配器来处理重载的运算符。
binaryOperator(hasOperatorName("+"), hasLHS(integerLiteral(equals(0))))

有三种不同的匹配器的基本类别：

节点匹配器：匹配特定类型的AST节点的匹配器。
缩小匹配器：匹配AST节点上的属性的匹配器。
遍历匹配器：允许在AST节点之间遍历的匹配器。

dump ast:
clang++ -Xclang -ast-dump hello.cpp
clang++ -Xclang -ast-view hello.cpp
clang -cc1 -ast-view hello.cpp
clang -emit-ast source.c
clang-check -ast-dump -ast-dump-filter=hello.cpp hello.cpp --
clang-check -ast-print  hello.cpp --(打印所有代码,包括头文件)

clang -Xclang -ast-dump -fsyntax-only input.cpp (打印位置和函数名称等信息)

clang-query来学习AstMachers:
1. 匹配main函数的定义:
match functionDecl(hasName('main'))
2. 匹配for循环带有初始化的
match forStmt(hasLoopInit(declStmt()))

3. 匹配带有++的for循环
match forStmt(hasIncrement(unaryOperator(hasOperatorName("++"))))
4. 匹配带有--的for循环
match forStmt(hasIncrement(unaryOperator(hasOperatorName("--"))))
forStmt有两个接口getCond(),getInit() 

5. 所有for定义初始化为零的新变量的语句 
StatementMatcher LoopMatcher =
  forStmt(hasLoopInit(declStmt(hasSingleDecl(varDecl(
    hasInitializer(integerLiteral(equals(0)))))))).bind("forLoop");

6. for循环中有条件的语句
match forStmt(hasCondition(anything()))

7. typedef
match typedefDecl()
match recordDecl(isStruct())

8. Clang AST由两个很flexible的类组成：Decl和bStmt。它们各自有很多子类，下面是一些例子：
    FunctionDecl: 函数原型或者函数定义
    BinaryOperator: 表达式，如(a+b)
    CallExpr: 函数调用，如foo(x)
大多数AST里面的类名字都很容易理解，如ForStmt, IfStmt以及ReturnStmt。
9. 如何操作AstMachers选中的单元:
//------------------------------------------------------------------------------
// AST matching sample. Demonstrates:
//
// * How to write a simple source tool using libTooling.
// * How to use AST matchers to find interesting AST nodes.
// * How to use the Rewriter API to rewrite the source code.
//
// Eli Bendersky (eliben@gmail.com)
// This code is in the public domain
//------------------------------------------------------------------------------
#include <string>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory MatcherSampleCategory("Matcher Sample");

class IfStmtHandler : public MatchFinder::MatchCallback {
public:
  IfStmtHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &Result) {
    // The matched 'if' statement was bound to 'ifStmt'.
    if (const IfStmt *IfS = Result.Nodes.getNodeAs<clang::IfStmt>("ifStmt")) {
      const Stmt *Then = IfS->getThen();
      Rewrite.InsertText(Then->getLocStart(), "// the 'if' part\n", true, true);

      if (const Stmt *Else = IfS->getElse()) {
        Rewrite.InsertText(Else->getLocStart(), "// the 'else' part\n", true,
                           true);
      }
    }
  }

private:
  Rewriter &Rewrite;
};

class IncrementForLoopHandler : public MatchFinder::MatchCallback {
public:
  IncrementForLoopHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &Result) {
    const VarDecl *IncVar = Result.Nodes.getNodeAs<VarDecl>("incVarName");
    Rewrite.InsertText(IncVar->getLocStart(), "/* increment */", true, true);
  }

private:
  Rewriter &Rewrite;
};

// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser. It registers a couple of matchers and runs them on
// the AST.
class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer(Rewriter &R) : HandlerForIf(R), HandlerForFor(R) {
    // Add a simple matcher for finding 'if' statements.
    Matcher.addMatcher(ifStmt().bind("ifStmt"), &HandlerForIf);

    // Add a complex matcher for finding 'for' loops with an initializer set
    // to 0, < comparison in the codition and an increment. For example:
    //
    //  for (int i = 0; i < N; ++i)
    Matcher.addMatcher(
        forStmt(hasLoopInit(declStmt(hasSingleDecl(
                    varDecl(hasInitializer(integerLiteral(equals(0))))
                        .bind("initVarName")))),
                hasIncrement(unaryOperator(
                    hasOperatorName("++"),
                    hasUnaryOperand(declRefExpr(to(
                        varDecl(hasType(isInteger())).bind("incVarName")))))),
                hasCondition(binaryOperator(
                    hasOperatorName("<"),
                    hasLHS(ignoringParenImpCasts(declRefExpr(to(
                        varDecl(hasType(isInteger())).bind("condVarName"))))),
                    hasRHS(expr(hasType(isInteger()))))))
            .bind("forLoop"),
        &HandlerForFor);
  }

  void HandleTranslationUnit(ASTContext &Context) override {
    // Run the matchers when we have the whole TU parsed.
    Matcher.matchAST(Context);
  }

private:
  IfStmtHandler HandlerForIf;
  IncrementForLoopHandler HandlerForFor;
  MatchFinder Matcher;
};

// For each source file provided to the tool, a new FrontendAction is created.
class MyFrontendAction : public ASTFrontendAction {
public:
  MyFrontendAction() {}
  void EndSourceFileAction() override {
    TheRewriter.getEditBuffer(TheRewriter.getSourceMgr().getMainFileID())
        .write(llvm::outs());
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return llvm::make_unique<MyASTConsumer>(TheRewriter);
  }

private:
  Rewriter TheRewriter;
};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, MatcherSampleCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());

  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}

10. 重载runhanshu
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Lex/Lexer.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/raw_ostream.h"
#include <system_error>

using namespace clang;
using namespace clang::ast_matchers;
using namespace llvm;
using clang::tooling::newFrontendActionFactory;
using clang::tooling::Replacement;
using clang::tooling::CompilationDatabase;

// FIXME: Pull out helper methods in here into more fitting places.

// Returns the text that makes up 'node' in the source.
// Returns an empty string if the text cannot be found.
template <typename T>
static std::string getText(const SourceManager &SourceManager, const T &Node) {
  SourceLocation StartSpellingLocation =
      SourceManager.getSpellingLoc(Node.getLocStart());
  SourceLocation EndSpellingLocation =
      SourceManager.getSpellingLoc(Node.getLocEnd());
  if (!StartSpellingLocation.isValid() || !EndSpellingLocation.isValid()) {
    return std::string();
  }
  bool Invalid = true;
  const char *Text =
      SourceManager.getCharacterData(StartSpellingLocation, &Invalid);
  if (Invalid) {
    return std::string();
  }
  std::pair<FileID, unsigned> Start =
      SourceManager.getDecomposedLoc(StartSpellingLocation);
  std::pair<FileID, unsigned> End =
      SourceManager.getDecomposedLoc(Lexer::getLocForEndOfToken(
          EndSpellingLocation, 0, SourceManager, LangOptions()));
  if (Start.first != End.first) {
    // Start and end are in different files.
    return std::string();
  }
  if (End.second < Start.second) {
    // Shuffling text with macros may cause this.
    return std::string();
  }
  return std::string(Text, End.second - Start.second);
}

// Return true if expr needs to be put in parens when it is an argument of a
// prefix unary operator, e.g. when it is a binary or ternary operator
// syntactically.
static bool needParensAfterUnaryOperator(const Expr &ExprNode) {
  if (dyn_cast<clang::BinaryOperator>(&ExprNode) ||
      dyn_cast<clang::ConditionalOperator>(&ExprNode)) {
    return true;
  }
  if (const CXXOperatorCallExpr *op =
          dyn_cast<CXXOperatorCallExpr>(&ExprNode)) {
    return op->getNumArgs() == 2 && op->getOperator() != OO_PlusPlus &&
           op->getOperator() != OO_MinusMinus && op->getOperator() != OO_Call &&
           op->getOperator() != OO_Subscript;
  }
  return false;
}

// Format a pointer to an expression: prefix with '*' but simplify
// when it already begins with '&'.  Return empty string on failure.
static std::string formatDereference(const SourceManager &SourceManager,
                                     const Expr &ExprNode) {
  if (const clang::UnaryOperator *Op =
          dyn_cast<clang::UnaryOperator>(&ExprNode)) {
    if (Op->getOpcode() == UO_AddrOf) {
      // Strip leading '&'.
      return getText(SourceManager, *Op->getSubExpr()->IgnoreParens());
    }
  }
  const std::string Text = getText(SourceManager, ExprNode);
  if (Text.empty())
    return std::string();
  // Add leading '*'.
  if (needParensAfterUnaryOperator(ExprNode)) {
    return std::string("*(") + Text + ")";
  }
  return std::string("*") + Text;
}

namespace {
class FixCStrCall : public ast_matchers::MatchFinder::MatchCallback {
public:
  FixCStrCall(tooling::Replacements *Replace) : Replace(Replace) {}

  virtual void run(const ast_matchers::MatchFinder::MatchResult &Result) {
    const CallExpr *Call = Result.Nodes.getStmtAs<CallExpr>("call");
    const Expr *Arg = Result.Nodes.getStmtAs<Expr>("arg");
    const bool Arrow = Result.Nodes.getStmtAs<MemberExpr>("member")->isArrow();
    // Replace the "call" node with the "arg" node, prefixed with '*'
    // if the call was using '->' rather than '.'.
    const std::string ArgText =
        Arrow ? formatDereference(*Result.SourceManager, *Arg)
              : getText(*Result.SourceManager, *Arg);
    if (ArgText.empty())
      return;

    Replacement Rep(*Result.SourceManager, Call, ArgText);
    llvm::errs() << "-- Replacement:\n";
    llvm::errs() << Rep.toString() << "\n";
    // Replace->insert(Replacement(*Result.SourceManager, Call, ArgText));
  }

private:
  tooling::Replacements *Replace;
};
} // end namespace

const char *StringConstructor =
    "::std::basic_string<char, std::char_traits<char>, std::allocator<char> >"
    "::basic_string";

const char *StringCStrMethod =
    "::std::basic_string<char, std::char_traits<char>, std::allocator<char> >"
    "::c_str";

cl::opt<std::string> BuildPath(cl::Positional, cl::desc("<build-path>"));

cl::list<std::string> SourcePaths(cl::Positional,
                                  cl::desc("<source0> [... <sourceN>]"),
                                  cl::OneOrMore);

int main(int argc, const char **argv) {
  llvm::sys::PrintStackTraceOnErrorSignal();
  std::unique_ptr<CompilationDatabase> Compilations(
      tooling::FixedCompilationDatabase::loadFromCommandLine(argc, argv));
  cl::ParseCommandLineOptions(argc, argv);
  if (!Compilations) {
    std::string ErrorMessage;
    Compilations =
        CompilationDatabase::loadFromDirectory(BuildPath, ErrorMessage);
    if (!Compilations)
      llvm::report_fatal_error(ErrorMessage);
  }
  tooling::RefactoringTool Tool(*Compilations, SourcePaths);
  ast_matchers::MatchFinder Finder;
  FixCStrCall Callback(&Tool.getReplacements());
  Finder.addMatcher(
      constructExpr(
          hasDeclaration(methodDecl(hasName(StringConstructor))),
          argumentCountIs(2),
          // The first argument must have the form x.c_str() or p->c_str()
          // where the method is string::c_str().  We can use the copy
          // constructor of string instead (or the compiler might share
          // the string object).
          hasArgument(
              0, id("call", memberCallExpr(
                                callee(id("member", memberExpr())),
                                callee(methodDecl(hasName(StringCStrMethod))),
                                on(id("arg", expr()))))),
          // The second argument is the alloc object which must not be
          // present explicitly.
          hasArgument(1, defaultArgExpr())),
      &Callback);
  Finder.addMatcher(
      constructExpr(
          // Implicit constructors of these classes are overloaded
          // wrt. string types and they internally make a StringRef
          // referring to the argument.  Passing a string directly to
          // them is preferred to passing a char pointer.
          hasDeclaration(
              methodDecl(anyOf(hasName("::llvm::StringRef::StringRef"),
                               hasName("::llvm::Twine::Twine")))),
          argumentCountIs(1),
          // The only argument must have the form x.c_str() or p->c_str()
          // where the method is string::c_str().  StringRef also has
          // a constructor from string which is more efficient (avoids
          // strlen), so we can construct StringRef from the string
          // directly.
          hasArgument(
              0, id("call", memberCallExpr(
                                callee(id("member", memberExpr())),
                                callee(methodDecl(hasName(StringCStrMethod))),
                                on(id("arg", expr())))))),
      &Callback);
  return Tool.runAndSave(newFrontendActionFactory(&Finder).get());
}

11. 通过重写FrontEndAction来修改源文件
#include <sstream>
#include <string>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");

class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer(Rewriter &R) {}

  // This gets called *while the source is being parsed* - the full AST does not
  // exist yet.
  bool HandleTopLevelDecl(DeclGroupRef DR) override {
    for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
      // llvm::errs() << "This is a toplevel decl!\n";
      //(*b)->dump();
    }
    return true;
  }

  // This gets called only when the full TU is completely parsed.
  void HandleTranslationUnit(ASTContext &Context) {
    llvm::errs() << "********* The whole TU *************\n";
    Context.getTranslationUnitDecl()->dump();

    llvm::errs() << "****** going over the decls stored in it:\n";
    for (auto *D : Context.getTranslationUnitDecl()->decls()) {
      llvm::errs() << "Decl in the TU:\n";
      D->dump();
      llvm::errs() << "Its start location is: '"
                   << D->getLocStart().printToString(Context.getSourceManager())
                   << "'\n";
    }
  }
};

class MyFrontendAction : public ASTFrontendAction {
public:
  MyFrontendAction() {}
  void EndSourceFileAction() override {
    SourceManager &SM = TheRewriter.getSourceMgr();
    llvm::errs() << "** EndSourceFileAction for: "
                 << SM.getFileEntryForID(SM.getMainFileID())->getName() << "\n";

    // Now emit the rewritten buffer.
    // TheRewriter.getEditBuffer(SM.getMainFileID()).write(llvm::outs());
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    SourceManager &SM = CI.getSourceManager();
    llvm::errs() << "** Creating AST consumer for: " << file << "\n";
    llvm::errs() << "  Main file ID: "
                 << SM.getFileEntryForID(SM.getMainFileID())->getName() << "\n";
    SM.PrintStats();
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return llvm::make_unique<MyASTConsumer>(TheRewriter);
  }

private:
  Rewriter TheRewriter;
};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}


12. 通过重载MatchCallback来dump match ast node
class LoopPrinter : public MatchFinder::MatchCallback {
public :
    virtual void run(const MatchFinder::MatchResult &Result) {
        if (const ForStmt *FS = Result.Nodes.getNodeAs<clang::ForStmt>("forLoop"))
            FS->dump();
    }
};


13. match 调用名称为::memset的函数
callExpr(callee(functionDecl(hasName("::memset"))))
更复杂的:
callExpr(callee(functionDecl(hasName("::memset"))),
         hasArgument(0, declRefExpr()),
         hasArgument(1, integerLiteral(equals(0))))

pointerType匹配指针类型的变量
callExpr(callee(functionDecl(hasName("::memset"))),
         hasArgument(0, declRefExpr(hasType(pointerType()))),
         hasArgument(1, integerLiteral(equals(0))),
         hasArgument(2, sizeOfExpr(has(declRefExpr(hasType(pointerType()))))))

14. clang-tidy是一个静态代码分析框架，支持C++/C/Objective-C。clang-tidy则是基于抽象语法树(AST)对源文件进行分析，结果更加准确，能检测的问题也更多，并且可以进行自动更正
clang-tidy -p . ptr.cpp -extra-arg=c++1z
clang-tidy介绍文档:https://hokein.github.io/clang-tools-tutorial/clang-tidy.html

15. clang-query -p <build-path>用于used to read a compile command database.


16. 通过获取预处理的数据来更改头文件 include 语句;
https://xaizek.github.io/2015-04-23/detecting-wrong-first-include/

