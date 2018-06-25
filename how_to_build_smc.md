xh@wxh-HP:~/source/can_parser/smc-git-code/net/sf/smc$ make Smc.jar
/usr/bin/java -jar ../../../bin/Smc.jar -java -g parser/SmcLexer.sm
/usr/bin/java -jar ../../../bin/Smc.jar -java -g parser/SmcParser.sm
make: *** No rule to make target '../../../lib/Java/statemap/FSMContext.class', needed by 'Smc.jar'.  Stop.
wxh@wxh-HP:~/source/can_parser/smc-git-code/net/sf/smc$ cd ../../../lib/
C/          C++/        CSharp/     Groovy/     Java/       JavaScript/ Lua/        ObjC/       Perl/       Php/        Python/     Ruby/       Scala/      Tcl/        VB/         
wxh@wxh-HP:~/source/can_parser/smc-git-code/net/sf/smc$ cd ../../../lib/Java
wxh@wxh-HP:~/source/can_parser/smc-git-code/lib/Java$ make statemap.jar
/usr/bin/javac -d . -g -source 1.7 -target 1.7 FSMContext.java State.java TransitionUndefinedException.java StateUndefinedException.java FSMContext7.java State7.java TransitionHandle.java
warning: [options] bootstrap class path not set in conjunction with -source 1.7
1 warning
/usr/bin/jar cvf ./statemap.jar statemap
added manifest
adding: statemap/(in = 0) (out= 0)(stored 0%)
adding: statemap/State.class(in = 1033) (out= 581)(deflated 43%)
adding: statemap/State7.class(in = 1923) (out= 894)(deflated 53%)
adding: statemap/TransitionUndefinedException.class(in = 545) (out= 331)(deflated 39%)
adding: statemap/StateUndefinedException.class(in = 530) (out= 326)(deflated 38%)
adding: statemap/FSMContext7.class(in = 8413) (out= 3616)(deflated 57%)
adding: statemap/FSMContext.class(in = 4629) (out= 2148)(deflated 53%)
adding: statemap/TransitionHandle.class(in = 658) (out= 370)(deflated 43%)
wxh@wxh-HP:~/source/can_parser/smc-git-code/lib/Java$ cd -
/home/wxh/source/can_parser/smc-git-code/net/sf/smc
wxh@wxh-HP:~/source/can_parser/smc-git-code/net/sf/smc$ make Smc.jar
make -C model all
make[1]: Entering directory '/home/wxh/source/can_parser/smc-git-code/net/sf/smc/model'
/usr/bin/javac -d . -g -source 1.7 -target 1.7 -Xlint:unchecked SmcAction.java SmcElement.java SmcFSM.java SmcGuard.java SmcMap.java SmcParameter.java SmcState.java SmcTransition.java SmcVisitor.java
warning: [options] bootstrap class path not set in conjunction with -source 1.7
1 warning
/usr/bin/jar cf SmcModel.jar \
		net/sf/smc/model/*.class
make[1]: Leaving directory '/home/wxh/source/can_parser/smc-git-code/net/sf/smc/model'
make -C generator all
make[1]: Entering directory '/home/wxh/source/can_parser/smc-git-code/net/sf/smc/generator'
/usr/bin/javac -d . -g -source 1.7 -target 1.7 -Xlint:unchecked -classpath ../model/SmcModel.jar SmcOptions.java SmcCGenerator.java SmcCodeGenerator.java SmcCppGenerator.java SmcCSharpGenerator.java SmcGraphGenerator.java SmcGroovyGenerator.java SmcHeaderGenerator.java SmcHeaderCGenerator.java SmcHeaderObjCGenerator.java SmcJavaGenerator.java SmcJava7Generator.java SmcLuaGenerator.java SmcObjCGenerator.java SmcPerlGenerator.java SmcPhpGenerator.java SmcPythonGenerator.java SmcRubyGenerator.java SmcScalaGenerator.java SmcTableGenerator.java SmcTclGenerator.java SmcVBGenerator.java SmcJSGenerator.java
warning: [options] bootstrap class path not set in conjunction with -source 1.7
1 warning
/usr/bin/jar cf SmcGenerator.jar \
		net/sf/smc/generator/*.class
make[1]: Leaving directory '/home/wxh/source/can_parser/smc-git-code/net/sf/smc/generator'
make -C parser all
make[1]: Entering directory '/home/wxh/source/can_parser/smc-git-code/net/sf/smc/parser'
/usr/bin/javac -d . -g -source 1.7 -target 1.7 -Xlint:unchecked -classpath ../../../../lib/Java/statemap.jar:../model/SmcModel.jar SmcLexerContext.java SmcParserContext.java SmcMessage.java SmcLexer.java SmcParser.java
warning: [options] bootstrap class path not set in conjunction with -source 1.7
1 warning
/usr/bin/jar cf SmcParser.jar \
		net/sf/smc/parser/*.class
make[1]: Leaving directory '/home/wxh/source/can_parser/smc-git-code/net/sf/smc/parser'
/usr/bin/javac -d . -g -source 1.7 -target 1.7 -Xlint:unchecked -classpath .:../../../lib/Java/statemap.jar SmcSyntaxChecker.java Smc.java model/SmcAction.java model/SmcElement.java model/SmcFSM.java model/SmcGuard.java model/SmcMap.java model/SmcParameter.java model/SmcState.java model/SmcTransition.java model/SmcVisitor.java parser/SmcLexerContext.java parser/SmcParserContext.java parser/SmcMessage.java parser/SmcLexer.java parser/SmcParser.java generator/SmcOptions.java generator/SmcCGenerator.java generator/SmcCodeGenerator.java generator/SmcCppGenerator.java generator/SmcCSharpGenerator.java generator/SmcGraphGenerator.java generator/SmcGroovyGenerator.java generator/SmcHeaderGenerator.java generator/SmcHeaderCGenerator.java generator/SmcHeaderObjCGenerator.java generator/SmcJavaGenerator.java generator/SmcJava7Generator.java generator/SmcLuaGenerator.java generator/SmcObjCGenerator.java generator/SmcPerlGenerator.java generator/SmcPhpGenerator.java generator/SmcPythonGenerator.java generator/SmcRubyGenerator.java generator/SmcScalaGenerator.java generator/SmcTableGenerator.java generator/SmcTclGenerator.java generator/SmcVBGenerator.java generator/SmcJSGenerator.java
warning: [options] bootstrap class path not set in conjunction with -source 1.7
1 warning
/usr/bin/jar cmf manifest.txt Smc.jar \
		net/sf/smc/*.class \
		net/sf/smc/model \
		net/sf/smc/parser \
		net/sf/smc/generator
/usr/bin/jar uf Smc.jar -C ../../../lib/Java statemap
wxh@wxh-HP:~/source/can_parser/smc-git-code/net/sf/smc$ 

