# GenerateIconsClass cmake file

include(FileRecurseRelative)

cmake_minimum_required(VERSION 3.16)

function(generateIconsClass CLASS_NAME ICON_FILES)

    set(args_option "")
    set(args_single SOURCE_DIR OUTPUT_DIR PREFIX)
    set(args_multi)

    cmake_parse_arguments(PARSE_ARGV 2 arg
        "${args_option}" "${args_single}" "${args_multi}"
    )
    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown/unexpected arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()

    # Create correct names
    string(TOUPPER ${CLASS_NAME} CLASS_NAME_UPPER)
    string(TOLOWER ${CLASS_NAME} CLASS_NAME_LOWER)

    unset(ICON_FILES_IN)
    allFileRecurseRelative(ICON_FILES_IN ${arg_SOURCE_DIR})
    set(${ICON_FILES} ${ICON_FILES_IN} PARENT_SCOPE)

    set(OUT_CONTENT_H "")
    # Write Header
    string(APPEND OUT_CONTENT_H
        "// File auto generated with CMake generateIconsClass.\n"
        "// Everything written here will be lost.\n\n"
        "#ifndef ${CLASS_NAME_UPPER}_H\n"
        "#define ${CLASS_NAME_UPPER}_H\n\n"
        "#include <QDefs>\n"
        "#include <QUtils>\n\n"
        "class ${CLASS_NAME} : public QObject,\n"
        "                      public QQmlSingleton<${CLASS_NAME}>\n"
        "{\n"
        "    Q_OBJECT\n"
        "    QML_ELEMENT\n"
        "    QML_SINGLETON\n\n"
        "protected:\n"
        "    friend QQmlSingleton<${CLASS_NAME}>\;\n"
        "    explicit ${CLASS_NAME}(QObject* parent = nullptr)\;\n\n"
        "public:\n"
        )

    foreach(ICON_FILE ${ICON_FILES_IN})
        get_filename_component(FILENAME_WE ${ICON_FILE} NAME_WE)
        get_filename_component(FILENAME ${ICON_FILE} NAME)
        get_filename_component(FILENAME_EXT ${ICON_FILE} LAST_EXT)

        set(PROPERTY_NAME_LIST ${FILENAME_WE})
        # Create a list
        string(REGEX REPLACE "[_ -]" ";" PROPERTY_NAME_LIST ${PROPERTY_NAME_LIST})

        set(PROPERTY_NAME "")
        set(PROPERTY_NAME_U "")

        set(PROPERTY_WORD_INDEX 0)
        foreach(PROPERTY_WORD ${PROPERTY_NAME_LIST})
            if(PROPERTY_WORD_INDEX EQUAL 0)
                string(SUBSTRING ${PROPERTY_WORD} 0 1 FIRST_LETTER)
                string(TOLOWER ${FIRST_LETTER} FIRST_LETTER)
                string(REGEX REPLACE "^.(.*)" "${FIRST_LETTER}\\1" PROPERTY_WORD "${PROPERTY_WORD}")

                string(TOUPPER ${FIRST_LETTER} FIRST_LETTER)
                string(REGEX REPLACE "^.(.*)" "${FIRST_LETTER}\\1" PROPERTY_WORD_U "${PROPERTY_WORD}")
            else()
                string(SUBSTRING ${PROPERTY_WORD} 0 1 FIRST_LETTER)
                string(TOUPPER ${FIRST_LETTER} FIRST_LETTER)
                string(REGEX REPLACE "^.(.*)" "${FIRST_LETTER}\\1" PROPERTY_WORD "${PROPERTY_WORD}")

                string(TOUPPER ${FIRST_LETTER} FIRST_LETTER)
                string(REGEX REPLACE "^.(.*)" "${FIRST_LETTER}\\1" PROPERTY_WORD_U "${PROPERTY_WORD}")
            endif()
            set(PROPERTY_NAME "${PROPERTY_NAME}${PROPERTY_WORD}")
            set(PROPERTY_NAME_U "${PROPERTY_NAME_U}${PROPERTY_WORD_U}")
            math(EXPR PROPERTY_WORD_INDEX "${PROPERTY_WORD_INDEX}+1")
        endforeach()

        set(FORBIDDEN_PROPERTY_WORDS id index model modelData console do if in for let new try var case else enum eval null this true void with await break catch class const false super throw while yield delete export import public return static switch typeof default extends finally package private continue debugger function arguments interface protected implements instanceof linux)

        if (PROPERTY_NAME IN_LIST FORBIDDEN_PROPERTY_WORDS)
            set(PROPERTY_NAME ${PROPERTY_NAME}_)
        endif()

        set(FILENAME_ABS "qrc:${arg_PREFIX}/${FILENAME}")
        string(APPEND OUT_CONTENT_H
        "    "
        "Q_CONSTANT_REF_PROPERTY(QString, ${PROPERTY_NAME}, ${PROPERTY_NAME_U}, \"${FILENAME_ABS}\")\n")
    endforeach()

    string(APPEND OUT_CONTENT_H
        "}\;\n\n"
        "#endif // ${CLASS_NAME_UPPER}_H\n"
    )

    get_filename_component(OUT_FILENAME ${arg_OUTPUT_DIR}/${CLASS_NAME_LOWER} ABSOLUTE)
    set(OUT_FILENAME_H ${OUT_FILENAME}.h)
    set(OUT_FILENAME_CPP ${OUT_FILENAME}.cpp)

    # Write file to temp then copy to original if there are diff
    file(WRITE ${OUT_FILENAME_H}.temp ${OUT_CONTENT_H})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OUT_FILENAME_H}.temp ${OUT_FILENAME_H})
    file(REMOVE ${OUT_FILENAME_H}.temp)

    set(OUT_CONTENT_CPP "")
    string(APPEND OUT_CONTENT_CPP
      "#include \"${CLASS_NAME_LOWER}.h\"\n"
      "\n"
      "${CLASS_NAME}::${CLASS_NAME}(QObject *parent):\n"
      "    QObject(parent)\n"
      "{\n"
      "\n"
      "}\n"
    )

    file(WRITE ${OUT_FILENAME_CPP}.temp ${OUT_CONTENT_CPP})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OUT_FILENAME_CPP}.temp ${OUT_FILENAME_CPP})
    file(REMOVE ${OUT_FILENAME_CPP}.temp)

endfunction()
