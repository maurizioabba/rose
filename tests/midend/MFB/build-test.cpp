
#include "MFB/Sage/driver.hpp"

#include "MFB/Sage/function-declaration.hpp"
#include "MFB/Sage/member-function-declaration.hpp"
#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/variable-declaration.hpp"

#include "sage3basic.h"

#include "AstConsistencyTests.h"

int main(int argc, char ** argv) {
  SgProject * project = new SgProject();
  std::vector<std::string> arglist;
    arglist.push_back("c++");
    arglist.push_back("-c");
  project->set_originalCommandLineArgumentList (arglist);

  MFB::Driver<MFB::Sage> driver(project);

// Create class A in files A.hpp and A.cpp
  unsigned lib_A_header_id = driver.add(boost::filesystem::path("A.hpp"));
  driver.setUnparsedFile(lib_A_header_id);
  unsigned lib_A_source_id = driver.add(boost::filesystem::path("A.cpp"));
  driver.setUnparsedFile(lib_A_source_id);
  
  SgClassSymbol * class_A_symbol = NULL;
  SgMemberFunctionSymbol * A_member_function_foo_symbol = NULL;
  SgMemberFunctionSymbol * A_member_function_bar_symbol = NULL;
  {
  // Class A
    MFB::Sage<SgClassDeclaration>::object_desc_t desc("A", (unsigned long)SgClassDeclaration::e_class, NULL, lib_A_file_id, true);

    // MFB::Sage<SgClassDeclaration>::build_result_t result = driver.build<SgClassDeclaration>(desc);
    class_A_symbol = driver.build<SgClassDeclaration>(desc).symbol;

  // Method A::foo()
    MFB::Sage<SgMemberFunctionDeclaration>::object_desc_t desc_foo(
      "foo",
      SageBuilder::buildVoidType(),
      SageBuilder::buildFunctionParameterList(),
      class_A_symbol
    );

    // MFB::Sage<SgMemberFunctionDeclaration>::build_result_t result = driver.build<SgMemberFunctionDeclaration>(desc_foo);
    A_member_function_foo_symbol = driver.build<SgMemberFunctionDeclaration>(desc_foo).symbol;

  // Method A::bar()
    MFB::Sage<SgMemberFunctionDeclaration>::object_desc_t desc_bar(
      "bar",
      SageBuilder::buildVoidType(),
      SageBuilder::buildFunctionParameterList(),
      class_A_symbol
    );

    // MFB::Sage<SgMemberFunctionDeclaration>::build_result_t result = driver.build<SgMemberFunctionDeclaration>(desc_bar);
    A_member_function_bar_symbol = driver.build<SgMemberFunctionDeclaration>(desc_bar).symbol;
  }

// Create class B in files B.hpp and B.cpp
  unsigned long lib_B_file_id = driver.createPairOfFiles("B");
  SgClassSymbol * class_B_symbol = NULL;
  SgMemberFunctionSymbol * B_ctor_symbol = NULL;
  {
  // Class B
    MFB::Sage<SgClassDeclaration>::object_desc_t desc("B", (unsigned long)SgClassDeclaration::e_class, NULL, lib_B_file_id, true);

    // MFB::Sage<SgClassDeclaration>::build_result_t result = driver.build<SgClassDeclaration>(desc);
    class_B_symbol = driver.build<SgClassDeclaration>(desc).symbol;

  // Reference type to class A in file B...
    SgType * class_A_ref_type = SageBuilder::buildReferenceType(driver.useSymbol<SgClassDeclaration>(class_A_symbol, lib_B_file_id, true, true)->get_type());

  // Field: A & B::a
    MFB::Sage<SgVariableDeclaration>::object_desc_t desc_var("a", class_A_ref_type, NULL, class_B_symbol, lib_B_file_id);
    MFB::Sage<SgVariableDeclaration>::build_result_t result_var = driver.build<SgVariableDeclaration>(desc_var);

  // Constructor B::B(A & a_)
    SgInitializedName * param_init_name = SageBuilder::buildInitializedName("a_", class_A_ref_type);
    MFB::Sage<SgMemberFunctionDeclaration>::object_desc_t desc_ctor(
      "B",
      SageBuilder::buildVoidType(),
      SageBuilder::buildFunctionParameterList(param_init_name),
      class_B_symbol,
      lib_B_file_id,
      false,
      false,
      true,
      false,
      true
    );

    MFB::Sage<SgMemberFunctionDeclaration>::build_result_t result = driver.build<SgMemberFunctionDeclaration>(desc_ctor);
    B_ctor_symbol = result.symbol;

    SgBasicBlock * ctor_body = result.definition->get_body();
    assert(ctor_body != NULL);

    SgVariableSymbol * var_symbol = result.definition->lookup_variable_symbol("a_");
    assert(var_symbol != NULL);

    SgExprStatement * expr_stmt = SageBuilder::buildExprStatement(
      SageBuilder::buildFunctionCallExp(
        SageBuilder::buildDotExp(
          SageBuilder::buildVarRefExp(var_symbol),
          SageBuilder::buildMemberFunctionRefExp(
            driver.useSymbol<SgMemberFunctionDeclaration>(A_member_function_bar_symbol, ctor_body),
            false,
            false
          )
        ),
        SageBuilder::buildExprListExp()
      )
    );
    SageInterface::appendStatement(expr_stmt, ctor_body);
  }

// Create Main in main.cpp
  unsigned long main_file_id = driver.add(boost::filesystem::path("main.cpp"));
  SgFunctionSymbol * main_symbol = NULL;
  {
    MFB::Sage<SgFunctionDeclaration>::object_desc_t desc(
      "main",
      SageBuilder::buildIntType(),
      SageBuilder::buildFunctionParameterList(),
      NULL,
      main_file_id
    );

    MFB::Sage<SgFunctionDeclaration>::build_result_t result = driver.build<SgFunctionDeclaration>(desc);

    main_symbol = result.symbol;

    SgScopeStatement * scope = result.definition->get_body();

    SgStatement * stmt = SageBuilder::buildVariableDeclaration(
                           "a",
                           driver.useSymbol<SgClassDeclaration>(class_A_symbol, scope)->get_type(),
                           NULL,
                           scope
                         );
    SageInterface::appendStatement(stmt, scope);

    SgVariableSymbol * var_symbol = scope->lookup_variable_symbol("a");
    assert(var_symbol != NULL);

    stmt = SageBuilder::buildExprStatement(
      SageBuilder::buildFunctionCallExp(
        SageBuilder::buildDotExp(
          SageBuilder::buildVarRefExp(var_symbol),
          SageBuilder::buildMemberFunctionRefExp(
            driver.useSymbol<SgMemberFunctionDeclaration>(A_member_function_foo_symbol, scope),
            false,
            false
          )
        ),
        SageBuilder::buildExprListExp()
      )
    );
    SageInterface::appendStatement(stmt, scope);
  }

//AstTests::runAllTests(project);

  driver.project->unparse();

  return 0;
}

