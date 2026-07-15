export module tona.ast;

import std;

import tona.type;
import tona.expr;
import tona.stmt;

export namespace Tona {

  struct VarDeclNode {
    StorageSpecifiers ss = StorageSpecifiers::SS_NONE;
    TypeQualifiers tq = TypeQualifiers::TQ_NONE;
    ExprNodeType init = ExprNodeType::ENT_NONE;
  };
  
}