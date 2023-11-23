#include "ast2llvm.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <cassert>
#include <list>

using namespace std;
using namespace LLVMIR;

static unordered_map<string,FuncType> funcReturnMap;
static unordered_map<string,StructInfo> structInfoMap;
static unordered_map<string,Name_name*> globalVarMap;
static unordered_map<string,Temp_temp*> localVarMap;
static list<L_stm*> emit_irs;

LLVMIR::L_prog* ast2llvm(aA_program p)
{
    auto defs = ast2llvmProg_first(p);
    auto funcs = ast2llvmProg_second(p);
    vector<L_func*> funcs_block;
    for(const auto &f : funcs)
    {
        funcs_block.push_back(ast2llvmFuncBlock(f));
    }
    for(auto &f : funcs_block)
    {
        ast2llvm_moveAlloca(f);
    }
    return new L_prog(defs,funcs_block);
}

int ast2llvmRightVal_first(aA_rightVal r)
{
    if(r == nullptr)
    {
        return 0;
    }
    switch (r->kind)
    {
    case A_arithExprValKind:
    {
        return ast2llvmArithExpr_first(r->u.arithExpr);
        break;
    }
    case A_boolExprValKind:
    {
        return ast2llvmBoolExpr_first(r->u.boolExpr);
        break;
    }
    default:
        break;
    }
    return 0;
}

int ast2llvmBoolExpr_first(aA_boolExpr b)
{
    switch (b->kind)
    {
    case A_boolBiOpExprKind:
    {
        return ast2llvmBoolBiOpExpr_first(b->u.boolBiOpExpr);
        break;
    }
    case A_boolUnitKind:
    {
        return ast2llvmBoolUnit_first(b->u.boolUnit);
        break;
    }
    default:
         break;
    }
    return 0;
}

int ast2llvmBoolBiOpExpr_first(aA_boolBiOpExpr b)
{
    int l = ast2llvmBoolExpr_first(b->left);
    int r = ast2llvmBoolExpr_first(b->right);
    if(b->op == A_and)
    {
        return l && r;
    }
    else
    {
        return l || r;
    }
}

int ast2llvmBoolUOpExpr_first(aA_boolUOpExpr b)
{
    if(b->op == A_not)
    {
        return !ast2llvmBoolUnit_first(b->cond);
    }
    return 0;
}

int ast2llvmBoolUnit_first(aA_boolUnit b)
{
    switch (b->kind)
    {
    case A_comOpExprKind:
    {
        return ast2llvmComOpExpr_first(b->u.comExpr);
        break;
    }
    case A_boolExprKind:
    {
        return ast2llvmBoolExpr_first(b->u.boolExpr);
        break;
    }
    case A_boolUOpExprKind:
    {
        return ast2llvmBoolUOpExpr_first(b->u.boolUOpExpr);
        break;
    }
    default:
        break;
    }
    return 0;
}

int ast2llvmComOpExpr_first(aA_comExpr c)
{
    auto l = ast2llvmExprUnit_first(c->left);
    auto r = ast2llvmExprUnit_first(c->right);
    switch (c->op)
    {
    case A_lt:
    {
        return l < r;
        break;
    }
    case A_le:
    {
        return l <= r;
        break;
    }
    case A_gt:
    {
        return l > r;
        break;
    }
    case A_ge:
    {
        return l >= r;
        break;
    }
    case A_eq:
    {
        return l == r;
        break;
    }
    case A_ne:
    {
        return l != r;
        break;
    }
    default:
        break;
    }
    return 0;
}

int ast2llvmArithBiOpExpr_first(aA_arithBiOpExpr a)
{
    auto l= ast2llvmArithExpr_first(a->left);
    auto r = ast2llvmArithExpr_first(a->right);
    switch (a->op)
    {
    case A_add:
    {
        return l + r;
        break;
    }
    case A_sub:
    {
        return l - r;
        break;
    }
    case A_mul:
    {
        return l * r;
        break;
    }
    case A_div:
    {
        return l / r;
        break;
    }
    default:
        break;
    }
    return 0;
}

int ast2llvmArithUExpr_first(aA_arithUExpr a)
{
    if(a->op == A_neg)
    {
        return -ast2llvmExprUnit_first(a->expr);
    }
    return 0;
}

int ast2llvmArithExpr_first(aA_arithExpr a)
{
    switch (a->kind)
    {
    case A_arithBiOpExprKind:
    {
        return ast2llvmArithBiOpExpr_first(a->u.arithBiOpExpr);
        break;
    }
    case A_exprUnitKind:
    {
        return ast2llvmExprUnit_first(a->u.exprUnit);
        break;
    }
    default:
        assert(0);
        break;
    }
    return 0;
}

int ast2llvmExprUnit_first(aA_exprUnit e)
{
    if(e->kind == A_numExprKind)
    {
        return e->u.num;
    }
    else if(e->kind == A_arithExprKind)
    {
        return ast2llvmArithExpr_first(e->u.arithExpr);
    }
    else if(e->kind == A_arithUExprKind)
    {
        return ast2llvmArithUExpr_first(e->u.arithUExpr);
    }
    else
    {
        assert(0);
    }
    return 0;
}

std::vector<LLVMIR::L_def*> ast2llvmProg_first(aA_program p)
{
    vector<L_def*> defs;
    defs.push_back(L_Funcdecl("getch",vector<TempDef>(),FuncType(ReturnType::INT_TYPE)));
    defs.push_back(L_Funcdecl("getint",vector<TempDef>(),FuncType(ReturnType::INT_TYPE)));
    defs.push_back(L_Funcdecl("putch",vector<TempDef>{TempDef(TempType::INT_TEMP)},FuncType(ReturnType::VOID_TYPE)));
    defs.push_back(L_Funcdecl("putint",vector<TempDef>{TempDef(TempType::INT_TEMP)},FuncType(ReturnType::VOID_TYPE)));
    defs.push_back(L_Funcdecl("putarray",vector<TempDef>{TempDef(TempType::INT_TEMP),TempDef(TempType::INT_PTR,-1)},FuncType(ReturnType::VOID_TYPE)));
    defs.push_back(L_Funcdecl("_sysy_starttime",vector<TempDef>{TempDef(TempType::INT_TEMP)},FuncType(ReturnType::VOID_TYPE)));
    defs.push_back(L_Funcdecl("_sysy_stoptime",vector<TempDef>{TempDef(TempType::INT_TEMP)},FuncType(ReturnType::VOID_TYPE)));
    for(const auto &v : p->programElements)
    {
        switch (v->kind)
        {
        case A_programNullStmtKind:
        {
            break;
        }
        case A_programVarDeclStmtKind:
        {
            if(v->u.varDeclStmt->kind == A_varDeclKind)
            {
                if(v->u.varDeclStmt->u.varDecl->kind == A_varDeclScalarKind)
                {
                    if(v->u.varDeclStmt->u.varDecl->u.declScalar->type->type == A_structTypeKind)
                    {
                        globalVarMap.emplace(*v->u.varDeclStmt->u.varDecl->u.declScalar->id,
                            Name_newname_struct(Temp_newlabel_named(*v->u.varDeclStmt->u.varDecl->u.declScalar->id),*v->u.varDeclStmt->u.varDecl->u.declScalar->type->u.structType));
                        TempDef def(TempType::STRUCT_TEMP,0,*v->u.varDeclStmt->u.varDecl->u.declScalar->type->u.structType);
                        defs.push_back(L_Globaldef(*v->u.varDeclStmt->u.varDecl->u.declScalar->id,def,vector<int>()));
                    }
                    else
                    {
                        globalVarMap.emplace(*v->u.varDeclStmt->u.varDecl->u.declScalar->id,
                            Name_newname_int(Temp_newlabel_named(*v->u.varDeclStmt->u.varDecl->u.declScalar->id)));
                        TempDef def(TempType::INT_TEMP,0);
                        defs.push_back(L_Globaldef(*v->u.varDeclStmt->u.varDecl->u.declScalar->id,def,vector<int>()));
                    }
                }
                else if(v->u.varDeclStmt->u.varDecl->kind == A_varDeclArrayKind)
                {
                    if(v->u.varDeclStmt->u.varDecl->u.declArray->type->type == A_structTypeKind)
                    {
                        globalVarMap.emplace(*v->u.varDeclStmt->u.varDecl->u.declArray->id,
                            Name_newname_struct_ptr(Temp_newlabel_named(*v->u.varDeclStmt->u.varDecl->u.declArray->id),v->u.varDeclStmt->u.varDecl->u.declArray->len,*v->u.varDeclStmt->u.varDecl->u.declArray->type->u.structType));
                        TempDef def(TempType::STRUCT_PTR,v->u.varDeclStmt->u.varDecl->u.declArray->len,*v->u.varDeclStmt->u.varDecl->u.declArray->type->u.structType);
                        defs.push_back(L_Globaldef(*v->u.varDeclStmt->u.varDecl->u.declArray->id,def,vector<int>()));
                    }
                    else
                    {
                        globalVarMap.emplace(*v->u.varDeclStmt->u.varDecl->u.declArray->id,
                            Name_newname_int_ptr(Temp_newlabel_named(*v->u.varDeclStmt->u.varDecl->u.declArray->id),v->u.varDeclStmt->u.varDecl->u.declArray->len));
                        TempDef def(TempType::INT_PTR,v->u.varDeclStmt->u.varDecl->u.declArray->len);
                        defs.push_back(L_Globaldef(*v->u.varDeclStmt->u.varDecl->u.declArray->id,def,vector<int>()));
                    }
                }
                else
                {
                    assert(0);
                }
            }
            else if(v->u.varDeclStmt->kind == A_varDefKind)
            {
                if(v->u.varDeclStmt->u.varDef->kind == A_varDefScalarKind)
                {
                    if(v->u.varDeclStmt->u.varDef->u.defScalar->type->type == A_structTypeKind)
                    {
                        globalVarMap.emplace(*v->u.varDeclStmt->u.varDef->u.defScalar->id,
                            Name_newname_struct(Temp_newlabel_named(*v->u.varDeclStmt->u.varDef->u.defScalar->id),*v->u.varDeclStmt->u.varDef->u.defScalar->type->u.structType));
                        TempDef def(TempType::STRUCT_TEMP,0,*v->u.varDeclStmt->u.varDef->u.defScalar->type->u.structType);
                        defs.push_back(L_Globaldef(*v->u.varDeclStmt->u.varDef->u.defScalar->id,def,vector<int>()));
                    }
                    else
                    {
                        globalVarMap.emplace(*v->u.varDeclStmt->u.varDef->u.defScalar->id,
                            Name_newname_int(Temp_newlabel_named(*v->u.varDeclStmt->u.varDef->u.defScalar->id)));
                        TempDef def(TempType::INT_TEMP,0);
                        vector<int> init;
                        init.push_back(ast2llvmRightVal_first(v->u.varDeclStmt->u.varDef->u.defScalar->val));
                        defs.push_back(L_Globaldef(*v->u.varDeclStmt->u.varDef->u.defScalar->id,def,init));
                    }
                }
                else if(v->u.varDeclStmt->u.varDef->kind == A_varDefArrayKind)
                {
                    if(v->u.varDeclStmt->u.varDef->u.defArray->type->type == A_structTypeKind)
                    {
                        globalVarMap.emplace(*v->u.varDeclStmt->u.varDef->u.defArray->id,
                            Name_newname_struct_ptr(Temp_newlabel_named(*v->u.varDeclStmt->u.varDef->u.defArray->id),v->u.varDeclStmt->u.varDef->u.defArray->len,*v->u.varDeclStmt->u.varDef->u.defArray->type->u.structType));
                        TempDef def(TempType::STRUCT_PTR,v->u.varDeclStmt->u.varDef->u.defArray->len,*v->u.varDeclStmt->u.varDef->u.defArray->type->u.structType);
                        defs.push_back(L_Globaldef(*v->u.varDeclStmt->u.varDef->u.defArray->id,def,vector<int>()));
                    }
                    else
                    {
                        globalVarMap.emplace(*v->u.varDeclStmt->u.varDef->u.defArray->id,
                            Name_newname_int_ptr(Temp_newlabel_named(*v->u.varDeclStmt->u.varDef->u.defArray->id),v->u.varDeclStmt->u.varDef->u.defArray->len));
                        TempDef def(TempType::INT_PTR,v->u.varDeclStmt->u.varDef->u.defArray->len);
                        vector<int> init;
                        for(auto &el : v->u.varDeclStmt->u.varDef->u.defArray->vals)
                        {
                            init.push_back(ast2llvmRightVal_first(el));
                        }
                        defs.push_back(L_Globaldef(*v->u.varDeclStmt->u.varDef->u.defArray->id,def,init));
                    }
                }
                else
                {
                    assert(0);
                }
            }
            else
            {
                assert(0);
            }
            break;
        }
        case A_programStructDefKind:
        {
            StructInfo si;
            int off = 0;
            vector<TempDef> members;
            for(const auto &decl : v->u.structDef->varDecls)
            {
                if(decl->kind == A_varDeclScalarKind)
                {
                    if(decl->u.declScalar->type->type == A_structTypeKind)
                    {
                        TempDef def(TempType::STRUCT_TEMP,0,*decl->u.declScalar->type->u.structType);
                        MemberInfo info(off++,def);
                        si.memberinfos.emplace(*decl->u.declScalar->id,info);
                        members.push_back(def);
                    }
                    else
                    {
                        TempDef def(TempType::INT_TEMP,0);
                        MemberInfo info(off++,def);
                        si.memberinfos.emplace(*decl->u.declScalar->id,info);
                        members.push_back(def);
                    }
                }
                else if(decl->kind == A_varDeclArrayKind)
                {
                    if(decl->u.declArray->type->type == A_structTypeKind)
                    {
                        TempDef def(TempType::STRUCT_PTR,decl->u.declArray->len,*decl->u.declArray->type->u.structType);
                        MemberInfo info(off++,def);
                        si.memberinfos.emplace(*decl->u.declArray->id,info);
                        members.push_back(def);
                    }
                    else
                    {
                        TempDef def(TempType::INT_PTR,decl->u.declArray->len);
                        MemberInfo info(off++,def);
                        si.memberinfos.emplace(*decl->u.declArray->id,info);
                        members.push_back(def);
                    }
                }
                else
                {
                    assert(0);
                }
            }
            structInfoMap.emplace(*v->u.structDef->id,std::move(si));
            defs.push_back(L_Structdef(*v->u.structDef->id,members));
            break;
        }
        case A_programFnDeclStmtKind:
        {
            FuncType type;
            if(v->u.fnDeclStmt->fnDecl->type == nullptr)
            {
                type.type = ReturnType::VOID_TYPE;
            }
            if(v->u.fnDeclStmt->fnDecl->type->type == A_nativeTypeKind)
            {
                type.type = ReturnType::INT_TYPE;
            }
            else if(v->u.fnDeclStmt->fnDecl->type->type == A_structTypeKind)
            {
                type.type = ReturnType::STRUCT_TYPE;
                type.structname = *v->u.fnDeclStmt->fnDecl->type->u.structType;
            }
            else
            {
                assert(0);
            }
            if(funcReturnMap.find(*v->u.fnDeclStmt->fnDecl->id) == funcReturnMap.end())
                funcReturnMap.emplace(*v->u.fnDeclStmt->fnDecl->id,std::move(type));
            vector<TempDef> args;
            for(const auto & decl : v->u.fnDeclStmt->fnDecl->paramDecl->varDecls)
            {
                if(decl->kind == A_varDeclScalarKind)
                {
                    if(decl->u.declScalar->type->type == A_structTypeKind)
                    {
                        TempDef def(TempType::STRUCT_PTR,0,*decl->u.declScalar->type->u.structType);
                        args.push_back(def);
                    }
                    else
                    {
                        TempDef def(TempType::INT_TEMP,0);
                        args.push_back(def);
                    }
                }
                else if(decl->kind == A_varDeclArrayKind)
                {
                    if(decl->u.declArray->type->type == A_structTypeKind)
                    {
                        TempDef def(TempType::STRUCT_PTR,-1,*decl->u.declArray->type->u.structType);
                        args.push_back(def);
                    }
                    else
                    {
                        TempDef def(TempType::INT_PTR,-1);
                        args.push_back(def);
                    }
                }
                else
                {
                    assert(0);
                }
            }
            defs.push_back(L_Funcdecl(*v->u.fnDeclStmt->fnDecl->id,args,type));
            break;
        }
        case A_programFnDefKind:
        {
            if(funcReturnMap.find(*v->u.fnDef->fnDecl->id) == funcReturnMap.end())
            {
                FuncType type;
                if(v->u.fnDef->fnDecl->type == nullptr)
                {
                    type.type = ReturnType::VOID_TYPE;
                }
                else if(v->u.fnDef->fnDecl->type->type == A_nativeTypeKind)
                {
                    type.type = ReturnType::INT_TYPE;
                }
                else if(v->u.fnDef->fnDecl->type->type == A_structTypeKind)
                {
                    type.type = ReturnType::STRUCT_TYPE;
                    type.structname = *v->u.fnDef->fnDecl->type->u.structType;
                }
                else
                {
                    assert(0);
                }
                funcReturnMap.emplace(*v->u.fnDef->fnDecl->id,std::move(type));
            }
            break;
        }
        default:
            assert(0);
            break;
        }
    }
    return defs;
}

std::vector<Func_local*> ast2llvmProg_second(aA_program p)
{
    vector<Func_local*> funcs;
    for (const auto &v: p->programElements){
        if (v->kind != A_programFnDefKind)
            continue;
        funcs.push_back(ast2llvmFunc(v->u.fnDef));    
    }
    return funcs;
}

Func_local* ast2llvmFunc(aA_fnDef f)
{
    FuncType functype;
    string name = *f->fnDecl->id;
    if(f->fnDecl->type == nullptr){
        functype.type = ReturnType::VOID_TYPE;
    }
    else if (f->fnDecl->type->type == A_nativeTypeKind){
        functype.type = ReturnType::INT_TYPE;
    }
    else if (f->fnDecl->type->type == A_structTypeKind){
        functype.type = ReturnType::STRUCT_TYPE;
        functype.structname = *f->fnDecl->type->u.structType;
    }
    else{
        assert(0);
    }
    vector<Temp_temp*> args;
    for(const auto & decl: f->fnDecl->paramDecl->varDecls){
        if(decl->kind == A_varDeclScalarKind){
            if(decl->u.declScalar->type->type == A_structTypeKind){
                auto def = Temp_newtemp_struct_ptr(0, *decl->u.declScalar->id);
                def->varname = *decl->u.declScalar->id;
                args.push_back(def);
            }
            else{
                auto def = Temp_newtemp_int();
                def->varname = *decl->u.declScalar->id;
                args.push_back(def);
            }
        }
        else if(decl->kind == A_varDeclArrayKind){
            if(decl->u.declArray->type->type == A_structTypeKind){
                auto def = Temp_newtemp_struct_ptr(-1, *decl->u.declArray->id);
                def->varname = *decl->u.declArray->id;
                args.push_back(def);
            }
            else{
                auto def = Temp_newtemp_int_ptr(-1);
                def->varname = *decl->u.declArray->id;
                args.push_back(def);
            }
        }
        else{
            assert(0);
        }
    }
    emit_irs.clear();
    emit_irs.emplace_back(L_Label(Temp_newlabel()));
    for(auto stmt:f->stmts){
        ast2llvmBlock(stmt);
    }

    list<L_stm*> ir = emit_irs;
    emit_irs.clear();
    return new Func_local(name, functype, args, ir);
}

void ast2llvmBlock(aA_codeBlockStmt b,Temp_label *con_label,Temp_label *bre_label)
{
    switch(b->kind){
        case A_codeBlockStmtType::A_nullStmtKind:{
            break;
        }
        case A_codeBlockStmtType::A_varDeclStmtKind:{
            
        }
        case A_codeBlockStmtType::A_assignStmtKind:{
            
        }
        case A_codeBlockStmtType::A_callStmtKind:{

        }
        case A_codeBlockStmtType::A_ifStmtKind:{

        }
        case A_codeBlockStmtType::A_whileStmtKind:{

        }
        case A_codeBlockStmtType::A_returnStmtKind:{

        }
        case A_codeBlockStmtType::A_continueStmtKind:{
            if(con_label == nullptr) assert(0);
            emit_irs.emplace_back(L_Jump(con_label));
            break;
        }
        case A_codeBlockStmtType::A_breakStmtKind:{
            if(bre_label == nullptr) assert(0);
            emit_irs.emplace_back(L_Jump(bre_label));
            break;
        }
        default:{
            assert(0);
            break;
        }
    }
}

AS_operand* ast2llvmRightVal(aA_rightVal r)
{
    if(r->kind == A_rightValType::A_arithExprValKind){
        return ast2llvmArithExpr(r->u.arithExpr);
    }else if (r->kind == A_rightValType::A_boolExprValKind)
    {
        return ast2llvmBoolExpr(r->u.boolExpr);
    }
    else{
        assert(0);
    }
    return nullptr;
}

AS_operand* ast2llvmLeftVal(aA_leftVal l)
{
    if (l->kind==A_leftValType::A_varValKind){
        string id=*l->u.id;
        if (localVarMap.find(id)!=localVarMap.end()){
            return AS_Operand_Temp(localVarMap[id]);
        } else if (globalVarMap.find(id)!=globalVarMap.end()){
            return AS_Operand_Name(globalVarMap[id]);
        }
        else assert(0);
    } else if (l->kind==A_leftValType::A_arrValKind){
        AS_operand* index=ast2llvmIndexExpr(l->u.arrExpr->idx);
        AS_operand* arr=ast2llvmLeftVal(l->u.arrExpr->arr);
        AS_operand* res;
        if (arr->kind==OperandKind::NAME){
            if (arr->u.NAME->type==TempType::INT_PTR){
                res=AS_Operand_Temp(Temp_newtemp_int_ptr(0));
            } else if (arr->u.NAME->type==TempType::STRUCT_PTR){
                res=AS_Operand_Temp(Temp_newtemp_struct_ptr(0,arr->u.NAME->structname));
            }
            else assert(0);
        } else if (arr->kind==OperandKind::TEMP){
            if (arr->u.TEMP->type==TempType::INT_PTR){
                res=AS_Operand_Temp(Temp_newtemp_int_ptr(0));
            } else if (arr->u.TEMP->type==TempType::STRUCT_PTR){
                res=AS_Operand_Temp(Temp_newtemp_struct_ptr(0,arr->u.NAME->structname));
            }
            else assert(0);
        }
        else assert(0);
        emit_irs.push_back(L_Gep(res,arr,index));
        return res;
    } else if (l->kind==A_leftValType::A_memberValKind){
        AS_operand* base=ast2llvmLeftVal(l->u.memberExpr->structId);
        AS_operand* res;
        AS_operand* index;
        if (base->kind==OperandKind::NAME){
            int offset=structInfoMap[base->u.NAME->structname].memberinfos[*l->u.memberExpr->memberId].offset;
            index=AS_Operand_Const(offset);
            TempDef def=structInfoMap[base->u.NAME->structname].memberinfos[*l->u.memberExpr->memberId].def;
            if (base->u.NAME->type==TempType::STRUCT_TEMP){
                if (def.kind==TempType::INT_TEMP){
                    res=AS_Operand_Temp(Temp_newtemp_int_ptr(0));
                } else if (def.kind==TempType::INT_PTR){
                    res=AS_Operand_Temp(Temp_newtemp_int_ptr(def.len));
                } else if (def.kind==TempType::STRUCT_TEMP){
                    res=AS_Operand_Temp(Temp_newtemp_struct_ptr(0,def.structname));
                } else if (def.kind==TempType::STRUCT_PTR){
                    res=AS_Operand_Temp(Temp_newtemp_struct_ptr(def.len,def.structname));
                }
            }
            else assert(0);
        } else if (base->kind==OperandKind::TEMP){
            int offset=structInfoMap[base->u.TEMP->structname].memberinfos[*l->u.memberExpr->memberId].offset;
            index=AS_Operand_Const(offset);
            TempDef def=structInfoMap[base->u.TEMP->structname].memberinfos[*l->u.memberExpr->memberId].def;
            if (base->u.TEMP->type==TempType::STRUCT_PTR && base->u.TEMP->len==0){
                if (def.kind==TempType::INT_TEMP){
                    res=AS_Operand_Temp(Temp_newtemp_int_ptr(0));
                } else if (def.kind==TempType::INT_PTR){
                    res=AS_Operand_Temp(Temp_newtemp_int_ptr(def.len));
                } else if (def.kind==TempType::STRUCT_TEMP){
                    res=AS_Operand_Temp(Temp_newtemp_struct_ptr(0,def.structname));
                } else if (def.kind==TempType::STRUCT_PTR){
                    res=AS_Operand_Temp(Temp_newtemp_struct_ptr(def.len,def.structname));
                }
            }
            else assert(0);
        }
        else assert(0);
        emit_irs.push_back(L_Gep(res,base,index));
        return res;
    }
    assert(0);
}

AS_operand* ast2llvmIndexExpr(aA_indexExpr index)
{
    if (index->kind==A_indexExprKind::A_numIndexKind){
        return AS_Operand_Const(index->u.num);
    } else if (index->kind==A_indexExprKind::A_idIndexKind){
        string id=*index->u.id;
        if (localVarMap.find(id)!=localVarMap.end()){
            AS_operand* res=AS_Operand_Temp(Temp_newtemp_int());
            AS_operand* mid=AS_Operand_Temp(localVarMap[id]);
            if (localVarMap[id]->type==TempType::INT_TEMP){
                return mid;
            }
            emit_irs.push_back(L_Load(res,mid));
            return res;
        } else if (globalVarMap.find(id)!=globalVarMap.end()){
            AS_operand* res=AS_Operand_Temp(Temp_newtemp_int());
            emit_irs.push_back(L_Load(res,AS_Operand_Name(globalVarMap[id])));
            return res;
        } else
            assert(0);
    } else{
        assert(0);
    }
}

AS_operand* ast2llvmBoolExpr(aA_boolExpr b,Temp_label *true_label,Temp_label *false_label)
{
    
}

void ast2llvmBoolBiOpExpr(aA_boolBiOpExpr b,Temp_label *true_label,Temp_label *false_label)
{
    
}

void ast2llvmBoolUnit(aA_boolUnit b,Temp_label *true_label,Temp_label *false_label)
{
    
}

void ast2llvmComOpExpr(aA_comExpr c,Temp_label *true_label,Temp_label *false_label)
{
    
}

AS_operand* ast2llvmArithBiOpExpr(aA_arithBiOpExpr a)
{
    AS_operand* l=ast2llvmArithExpr(a->left);
    AS_operand* r=ast2llvmArithExpr(a->right);
    AS_operand* dst=AS_Operand_Temp(Temp_newtemp_int());
    if(a->op == A_add){
        emit_irs.push_back(L_Binop(L_binopKind::T_plus,l,r,dst));
        return dst;
    }
    else if (a->op == A_sub)
    {
        emit_irs.push_back(L_Binop(L_binopKind::T_minus,l,r,dst));
        return dst;
    }
    else if (a->op == A_mul)
    {
        emit_irs.push_back(L_Binop(L_binopKind::T_mul,l,r,dst));
        return dst;
    }
    else if (a->op == A_div)
    {
        emit_irs.push_back(L_Binop(L_binopKind::T_div,l,r,dst));
        return dst;
    }
    else{
        assert(0);
    }
    return nullptr;
}

AS_operand* ast2llvmArithUExpr(aA_arithUExpr a)
{
    AS_operand* expr=ast2llvmExprUnit(a->expr);
    if (a->op == A_neg){
        Temp_temp* temp=Temp_newtemp_int();
        AS_operand* dst=AS_Operand_Temp(temp);
        emit_irs.push_back(L_Binop(L_binopKind::T_minus,AS_Operand_Const(0),expr,dst));
        return dst;
    }
    assert(0);
}

AS_operand* ast2llvmArithExpr(aA_arithExpr a){
    if(a->kind == A_arithExprType::A_arithBiOpExprKind){
        return ast2llvmArithBiOpExpr(a->u.arithBiOpExpr);
    }
    else if (a->kind == A_arithExprType::A_exprUnitKind)
    {
        return ast2llvmExprUnit(a->u.exprUnit);
    }
    else{
        assert(0);
    }
    return nullptr;
}

AS_operand* ast2llvmExprUnit(aA_exprUnit e)
{
    if(e->kind == A_exprUnitType::A_arithExprKind){
        return ast2llvmArithExpr(e->u.arithExpr);
    }else if (e->kind == A_exprUnitType::A_arithUExprKind)
    {
        return ast2llvmArithUExpr(e->u.arithUExpr);
    }else if (e->kind == A_exprUnitType::A_fnCallKind)
    {
        vector<AS_operand*> args;
        for(auto val : e->u.callExpr->vals){
            args.push_back(ast2llvmRightVal(val));
        }
        auto dst = AS_Operand_Temp(Temp_newtemp_int());
        emit_irs.push_back(L_Call(*e->u.callExpr->fn, dst, args));
    }
    else if (e->kind == A_exprUnitType::A_numExprKind)
    {
        return AS_Operand_Const(e->u.num);
    }
    else if (e->kind == A_exprUnitType::A_idExprKind)
    {
        AS_operand* res=AS_Operand_Temp(Temp_newtemp_int());
        if (localVarMap.find(*e->u.id)!=localVarMap.end()){
            AS_operand* temp=AS_Operand_Temp(localVarMap[*e->u.id]);
            if (localVarMap[*e->u.id]->type==TempType::INT_PTR){
                emit_irs.push_back(L_Load(res,temp));
                return res;
            } else if (localVarMap[*e->u.id]->type==TempType::INT_TEMP){
                return temp;
            }
            else assert(0);
        } else if (globalVarMap.find(*e->u.id)!=globalVarMap.end()){
            if (globalVarMap[*e->u.id]->type==TempType::INT_TEMP){
                emit_irs.push_back(L_Load(res,AS_Operand_Name(globalVarMap[*e->u.id])));
                return res;
            }
            else assert(0);
        }
    }
    else if (e->kind == A_exprUnitType::A_arrayExprKind)
    {
        AS_operand* index=ast2llvmIndexExpr(e->u.arrayExpr->idx);
        AS_operand* base=ast2llvmLeftVal(e->u.arrayExpr->arr);
        AS_operand* res=AS_Operand_Temp(Temp_newtemp_int());
        AS_operand* mid=AS_Operand_Temp(Temp_newtemp_int_ptr(0));
        emit_irs.push_back(L_Gep(mid,base,index));
        emit_irs.push_back(L_Load(res,mid));
        return res;
    }
    else if (e->kind == A_exprUnitType::A_memberExprKind)
    {
        AS_operand* struc=ast2llvmLeftVal(e->u.memberExpr->structId);
        AS_operand* mid=AS_Operand_Temp(Temp_newtemp_int_ptr(0));
        AS_operand* res=AS_Operand_Temp(Temp_newtemp_int());
        if (struc->kind==OperandKind::NAME){
            string structName=struc->u.NAME->structname;
            int offset=structInfoMap[structName].memberinfos[*e->u.memberExpr->memberId].offset;
            emit_irs.push_back(L_Gep(mid,struc,AS_Operand_Const(offset)));
            emit_irs.push_back(L_Load(res,mid));
            return res;
        } else if (struc->kind==OperandKind::TEMP){
            string structName=struc->u.TEMP->structname;
            int offset=structInfoMap[structName].memberinfos[*e->u.memberExpr->memberId].offset;
            emit_irs.push_back(L_Gep(mid,struc,AS_Operand_Const(offset)));
            emit_irs.push_back(L_Load(res,mid));
            return res;
        }
    }
    else{
        assert(0);
    }
    return nullptr;
}

LLVMIR::L_func* ast2llvmFuncBlock(Func_local *f)
{
    list<L_block*> blocks;
    assert(f->irs.front()->type == L_StmKind::T_LABEL);
    list<L_stm*> ins;
    for (auto ir : f->irs){
        if(ir->type == L_StmKind::T_LABEL && !ins.empty()){
            blocks.emplace_back(L_Block(ins));
            ins.clear();
        }
        ins.emplace_back(ir);
    }
    if(!ins.empty()){
        blocks.emplace_back(L_Block(ins));
        ins.clear();
    }
    return new L_func(f->name,f->ret,f->args,blocks);
}

void ast2llvm_moveAlloca(LLVMIR::L_func *f)
{
    
}