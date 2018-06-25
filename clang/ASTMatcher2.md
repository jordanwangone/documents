1. 匹配函数定义
match functionDecl()

2.把match的node绑定到字符串,之后在match callback中使用
match functionDecl().bind("func")
recordDecl(hasName("MyClass")).bind("id") will bind the matched recordDecl node to the string “id”, to be later retrieved in the match callback.

2. 匹配取地址符号,unary(一元)
match unaryOperator(hasOperatorName("&"))

3. 名字为Foo的类,且继承自Bar
recordDecl(hasName("Foo"), isDerivedFrom("Bar"))

4. 匹配void (*f)(void) = &func
(1)    unaryOperator(            // any unary operator, e.g. *, &, --
        hasOperatorName("&"), // exact unary operator: &
        declRefExpr(          // referencing a variable/declaration
            to(               // something that is ...
                functionDecl( // ... a function

                ).bind("ref") // bind matched function ref to "ref" name
            )
        )
    ).bind("op");             // bind matched unary op to "op" name
(2) static StatementMatcher funcRef =
    declRefExpr(              // referencing a variable/declaration
        to(                   // something that is ...
            functionDecl(     // ... a function

            )
        )
    ).bind("ref");            // bind matched function ref to "ref" name

5. libtooling实例
https://github.com/jason-heo/clang-libtooling-example
https://github.com/kevinaboos/LibToolingExample
https://github.com/su2278370/Libtooling
https://github.com/shigeki-akiyama/libtooling-example
https://github.com/nicolas17/my-clang-tools
https://github.com/jason-heo/clang-libtooling-example
https://github.com/frodrd00/ClangExamples
https://github.com/Artalus/odrey
https://github.com/cleversonledur/clang_examples
https://github.com/zayac/derive-terms
https://github.com/jfultz/libtooling_step_by_step
https://github.com/dillonhuff/clang-libtooling-example
https://github.com/trucnguyenlam/libtooling-template
https://github.com/JeroenBogersPhilips/refactor-tool
https://github.com/hach-que/ProjectNormalizer
https://github.com/seahorn/seatooling
https://github.com/firolino/clang-tool
https://github.com/TaoranJ/CodingAssistant-Clang
https://github.com/jszhujun2010/Clang-Basic-Tutorial

https://github.com/new32/east
https://github.com/amaiorano/hsm-analyze

稍复杂的实例:
https://github.com/Leandros/metareflect
https://github.com/lszero/crulet
自动生成代码:https://github.com/flexferrum/autoprogrammer
https://github.com/R1tschY/refactor-cpp

6. 匹配如下信息
serdes_config_t mirror_config = {
    .direction = 1 << DIRECTION_CDC,
    .topic_ids = mirror_topic_ids,
    .topic_len = sizeof(mirror_topic_ids) / sizeof(int),
    .id_event_maps = NULL,
    .id_event_map_len = 0,
    .update_cache = true,
    .deal_decode_data = deal_decode_data_mirror,
    .ops = &general_reqsts_serdes_ops,
};

match varDecl(hasType(asString("serdes_config_t")), has(initListExpr()))
