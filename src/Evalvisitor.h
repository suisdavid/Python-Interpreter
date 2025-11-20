#pragma once
#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include "Python3ParserBaseVisitor.h"
#include "int2048.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <iostream>
using namespace sjtu;
struct none{none()=default;};
struct constant
{
  int constid;
  std::any ret_val;
  constant(int id=0,std::any x=none())
  {
    constid=id;ret_val=x;
  }
};
struct arg
{
  std::string argname;
  std::any init_val;
  arg(std::string _argname,std::any _init_val)
  {
    argname=_argname;init_val=_init_val;
  }
};
class Scope
{
    public:
      std::unordered_map<std::string,int>funcid;
      std::unordered_map<std::string,int>varid;
      std::vector<std::any>var_val;
      std::vector<std::vector<arg> >funcarg;
      std::vector<Python3Parser::SuiteContext *>funcpos;
      int func_cnt,var_cnt;
      Scope(){func_cnt=var_cnt=0;}
      void varRegister(std::string varname,std::any val)
      {
        if (!varid.count(varname))
        {
          varid[varname]=var_cnt++;
          var_val.push_back(val);
        }
        else
        {
          var_val[varid[varname]]=val;
        }
      }
      std::any varGet(std::string varname)
      {
        return var_val[varid[varname]];
      }
      void funcRegister(std::string funcname,std::vector<arg>arglist,Python3Parser::SuiteContext * ctx)
      {
        if (!funcid.count(funcname))
        {
          funcid[funcname]=func_cnt++;
          funcarg.push_back(arglist);
          funcpos.push_back(ctx);
        }
        else
        {
          funcarg[funcid[funcname]]=arglist;
          funcpos[funcid[funcname]]=ctx;
        }
      }
};
class EvalVisitor : public Python3ParserBaseVisitor
 {
	// TODO: override all methods of Python3ParserBaseVisitor
  Scope scope;
std::any disarg(std::any x)
{
  if (x.type()==typeid(arg))
  {
    return std::any_cast<arg>(x).init_val;
  }
  if (x.type()==typeid(constant))
  {
    return std::any_cast<constant>(x).ret_val;
  }
  return x;
}
bool has(std::string s,char c)
{
  int len=s.length();
  for (int i=0;i<len;i++)
  {
    if (s[i]==c){return 1;}
  }
  return 0;
}
int2048 toint2048(std::any x)
{
  x=disarg(x);
  if (x.type()==typeid(int2048))
  {
    return std::any_cast<int2048>(x);
  }
  if (x.type()==typeid(bool))
  {
    bool y=std::any_cast<bool>(x);
    return int2048(y);
  }
  if (x.type()==typeid(double))
  {
    double y=std::any_cast<double>(x);
    return int2048(y);
  }
  if (x.type()==typeid(std::string))
  {
    std::string s=std::any_cast<std::string>(x);
    int len=s.length();int2048 res=0;
    if (s[0]=='-')
    {
      for (int i=1;i<len;i++)
      {
        res=res*10+(s[i]-'0');
      }
      return -res;
    }
    for (int i=0;i<len;i++)
    {
      res=res*10+(s[i]-'0');
    }
    return res; 
  }
}
double todouble(std::any x)
{
  x=disarg(x);
  if (x.type()==typeid(double))
  {
    return std::any_cast<double>(x);
  }
  if (x.type()==typeid(int2048))
  {
    int2048 y=std::any_cast<int2048>(x);
    return y.todouble();
  }
  if (x.type()==typeid(bool))
  {
    bool y=std::any_cast<bool>(x);
    return double(y);
  }
  if (x.type()==typeid(std::string))
  {
    int intpart=0,flg=1;double res=0,pw=1.0;
    std::string s=std::any_cast<std::string>(x);
    int len=s.length();
    if (s[0]=='-')
    {
      for (int i=1;i<len;i++)
      {
        if (s[i]=='.'){flg=0;continue;}
        if (flg)
        {
          intpart=intpart*10+(s[i]-'0');
        }
        else
        {
          pw/=10;res+=pw*(s[i]-'0');
        }
      }
    return -(res+intpart);
    }
    for (int i=0;i<len;i++)
    {
      if (s[i]=='.'){flg=0;continue;}
      if (flg)
      {
        intpart=intpart*10+(s[i]-'0');
      }
      else
      {
        pw/=10;res+=pw*(s[i]-'0');
      }
    }
    return res+intpart;
  }
}
std::string tostring(std::any x)
{
  x=disarg(x);
  if (x.type()==typeid(std::string))
  {
    return std::any_cast<std::string>(x);
  }
  if (x.type()==typeid(double))
  {
    double y=std::any_cast<double>(x);
    std::string res="";
    if (y<0){y=-y;res="-";}
    int intpart=(int)y;
    if (intpart==0){res+="0";}
    std::vector<int>digit;
    while (intpart>0)
    {
      digit.push_back(intpart%10);intpart/=10;
    }
    int sz=digit.size();intpart=(int)y;
    for (int i=sz-1;i>=0;i--){res+=(digit[i]+'0');}
    res+='.';
    for (int i=1;i<=6;i++)
    {
      y=(y-intpart)*10;intpart=(int)y;
      res+=(intpart+'0');
    }
    return res;
  }
  if (x.type()==typeid(int2048))
  {
    int2048 y=std::any_cast<int2048>(x);std::string res="";
    if (y==0){return res;}
    if (y<0){y=-y;res="-";}
    std::vector<int>digit;
    while (y>0)
    {
      digit.push_back((y%10).toint());y/=10;
    }
    int sz=digit.size();
    for (int i=sz-1;i>=0;i--){res+=(digit[i]+'0');}
    return res;
  }
  if (x.type()==typeid(bool))
  {
    bool y=std::any_cast<bool>(x);
    std::string res=(y?"True":"False");
    return res;
  }
}
int toint(std::string s)
{
  int len=s.length(),res=0;
  for (int i=0;i<len;i++)
  {
    res=res*10+(s[i]-'0');
  }
  return res;
}
bool tobool(std::any x)
{
  x=disarg(x);
  if (x.type()==typeid(int2048))
  {
    int2048 y=std::any_cast<int2048>(x);
    return y!=0;
  }
  if (x.type()==typeid(double))
  {
    double y=std::any_cast<double>(x);
    return y!=0;
  }
  if (x.type()==typeid(none))
  {
    return 0;
  }
  if (x.type()==typeid(bool))
  {
    return std::any_cast<bool>(x);
  }
  if (x.type()==typeid(std::string))
  {
    std::string y=std::any_cast<std::string>(x);
    return y.length();
  }
  return 1;
}
   std::any funcCalc(std::string funcname,std::vector<arg>arglist)
      {
        if (funcname=="print")
        {
          int sz=arglist.size();
          for (int i=0;i<sz;i++)
          {
            std::any x=arglist[i].init_val;
            if (x.type()==typeid(std::string))
            {
              std::cout<<tostring(x)<<" ";
            }
            else if (x.type()==typeid(int2048))
            {
              std::cout<<toint2048(x)<<" ";
            }
            else if (x.type()==typeid(bool))
            {
              std::cout<<(tobool(x)?"True":"False")<<" ";
            }
            else if (x.type()==typeid(double))
            {
              std::cout<<std::fixed<<std::setprecision(6)<<todouble(x)<<" ";
            }
            else if (x.type()==typeid(none))
            {
              std::cout<<"None"<<" ";
            }
          }
          std::cout<<std::endl;
          return none();
        }
        else if (funcname=="int")
        {
          return toint2048(arglist[0].init_val);
        }
        else if (funcname=="float")
        {
          return todouble(arglist[0].init_val);
        }
        else if (funcname=="str")
        {
          return tostring(arglist[0].init_val);
        }
        else if (funcname=="bool")
        {
          return tobool(arglist[0].init_val);
        }
        std::vector<arg>replace;
        std::unordered_map<std::string,bool>in_arg;
        int sz=arglist.size(),id=scope.funcid[funcname];
        for (int i=0;i<sz;i++)
        {
          arg v=arglist[i];
          if (v.argname=="")
          {
              v.argname=scope.funcarg[id][i].argname;
          }
          in_arg[v.argname]=1;
          if (scope.varid.count(v.argname))
          {
              int id=scope.varid[v.argname];
              replace.push_back(arg(v.argname,scope.var_val[id]));
              scope.var_val[id]=v.init_val;
          }
          else
          {
              scope.varRegister(v.argname,v.init_val);
          }
        }
        sz=scope.funcarg[id].size();
        for (int i=0;i<sz;i++)
        {
          arg v=scope.funcarg[id][i];
          if (!in_arg.count(v.argname))
          {
            if (scope.varid.count(v.argname))
            {
                int id=scope.varid[v.argname];
                replace.push_back(arg(v.argname,scope.var_val[id]));
                scope.var_val[id]=v.init_val;
            }
            else
            {
                scope.varRegister(v.argname,v.init_val);
            }
          }
        }
        in_arg.clear();
        std::any res=disarg(visitSuite(scope.funcpos[id]));
        sz=replace.size();
        for (int i=0;i<sz;i++)
        {
          arg v=replace[i];
          int id=scope.varid[v.argname];
          scope.var_val[id]=v.init_val;
        }
        return res;
      }
	virtual std::any visitFile_input(Python3Parser::File_inputContext *ctx) override {
     std::vector<Python3Parser::StmtContext *>v=ctx->stmt();
     int sz=v.size();
     for (int i=0;i<sz;i++)
     {
      visitStmt(v[i]);
     }
     return none();
  }

  virtual std::any visitFuncdef(Python3Parser::FuncdefContext *ctx) override {
    std::string funcname=ctx->NAME()->getText();
    std::vector<arg> arglist=std::any_cast<std::vector<arg> >(visitParameters(ctx->parameters()));
    scope.funcRegister(funcname,arglist,ctx->suite());
    return none();
  }

  virtual std::any visitParameters(Python3Parser::ParametersContext *ctx) override {
    if (ctx->typedargslist())
    {
      return visitTypedargslist(ctx->typedargslist());
    }
    std::vector<arg>v;return v;
  }

  virtual std::any visitTypedargslist(Python3Parser::TypedargslistContext *ctx) override {
    std::vector<Python3Parser::TfpdefContext *> vargname=ctx->tfpdef();
    std::vector<Python3Parser::TestContext *> vargval=ctx->test();
    int sz1=vargname.size(),sz2=vargval.size();
    std::vector<arg>arglist;
    for (int i=0;i<sz1-sz2;i++)
    {
      arglist.push_back(arg(std::any_cast<arg>(visitTfpdef(vargname[i])).argname,none()));
    }
    for (int i=sz1-sz2;i<sz1;i++)
    {
      arglist.push_back(arg(std::any_cast<arg>(visitTfpdef(vargname[i])).argname,disarg(visitTest(vargval[i-(sz1-sz2)]))));
    }
    return arglist;
  }

  virtual std::any visitTfpdef(Python3Parser::TfpdefContext *ctx) override {
      std::string argname=ctx->NAME()->getText();
      if (!scope.varid.count(argname))
      {
        scope.varRegister(argname,none());
      }
      return arg(argname,scope.varGet(argname));
  }

  virtual std::any visitStmt(Python3Parser::StmtContext *ctx) override {
    if (ctx->simple_stmt()){return visitSimple_stmt(ctx->simple_stmt());}
    if (ctx->compound_stmt()){return visitCompound_stmt(ctx->compound_stmt());}
  }

  virtual std::any visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) override {
    return visitSmall_stmt(ctx->small_stmt());
  }

  virtual std::any visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) override {
    if (ctx->expr_stmt()){return visitExpr_stmt(ctx->expr_stmt());}
    else if (ctx->flow_stmt()){return visitFlow_stmt(ctx->flow_stmt());}
  }

  virtual std::any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override {
    std::vector<Python3Parser::TestlistContext *>v=ctx->testlist();
    if (ctx->augassign())
    {
      int op=std::any_cast<int>(visitAugassign(ctx->augassign()));
      arg var1=std::any_cast<arg>(visitTestlist(v[0]));
      std::any val1=var1.init_val,val2=disarg(visitTestlist(v[1]));
      if (op==(int)Python3Parser::ADD_ASSIGN)
      {
        if (val1.type()==typeid(std::string))
        {
          scope.varRegister(var1.argname,tostring(val1)+tostring(val2));
        }
        else if (val1.type()==typeid(double)||val2.type()==typeid(double))
        {
          scope.varRegister(var1.argname,todouble(val1)+todouble(val2));
        }
        else
        {
          scope.varRegister(var1.argname,toint2048(val1)+toint2048(val2));
        }
      }
      else if (op==(int)Python3Parser::SUB_ASSIGN)
      {
        if (val1.type()==typeid(double)||val2.type()==typeid(double))
        {
          scope.varRegister(var1.argname,todouble(val1)-todouble(val2));
        }
        else
        {
          scope.varRegister(var1.argname,toint2048(val1)-toint2048(val2));
        }
      }
      else if (op==(int)Python3Parser::MULT_ASSIGN)
      {
        if (val1.type()==typeid(std::string))
        {
          std::string ans="",s=std::any_cast<std::string>(val1);
          int k=std::any_cast<int>(val2);
          for (int i=1;i<=k;i++){ans+=s;}
          scope.varRegister(var1.argname,ans);
        }
        else if (val1.type()==typeid(double)||val2.type()==typeid(double))
        {
          scope.varRegister(var1.argname,todouble(val1)*todouble(val2));
        }
        else
        {
          scope.varRegister(var1.argname,toint2048(val1)*toint2048(val2));
        }
      }
      else if (op== (int)Python3Parser::DIV_ASSIGN)
      { 
        scope.varRegister(var1.argname,todouble(val1)/todouble(val2));
      }
      else if (op==(int)Python3Parser::IDIV_ASSIGN)
      {
        if (val1.type()==typeid(double)||val2.type()==typeid(double))
        {
            scope.varRegister(var1.argname,double(int(todouble(val1)/todouble(val2))));
        }
        else
        {
          scope.varRegister(var1.argname,toint2048(val1)/toint2048(val2));
        }
      }
      else if (op=(int)Python3Parser::MOD_ASSIGN)
      {
        if (val1.type()==typeid(double)||val2.type()==typeid(double))
        {
          double x=todouble(val1),y=todouble(val2);
          scope.varRegister(var1.argname,x-y*int2048(x/y));
        }
        else
        {
          scope.varRegister(var1.argname,toint2048(val1)%toint2048(val2));
        }
      }
      return none();
    }
    else
    {
      int sz=v.size();
      if (sz==1){return visitTestlist(v[0]);}
      std::any t=visitTestlist(v[sz-1]);
      if (t.type()==typeid(std::vector<std::any>))
      {
        std::vector<std::any>res=std::any_cast<std::vector<std::any> >(t);
        for (int i=sz-2;i>=0;i--)
        {
          std::vector<std::any>vars=std::any_cast<std::vector<std::any> >(visitTestlist(v[i]));
          int varcnt=vars.size();
          for (int j=0;j<varcnt;j++)
          {
            arg x=std::any_cast<arg>(vars[j]);
            scope.varRegister(x.argname,disarg(res[j]));
          }
        }
      }
      else
      {
       for (int i=sz-2;i>=0;i--)
        {
          arg x=std::any_cast<arg>(visitTestlist(v[i]));
          scope.varRegister(x.argname,disarg(t));
        }
      }
      return none();
    }
  }

  virtual std::any visitAugassign(Python3Parser::AugassignContext *ctx) override {
   if (ctx->ADD_ASSIGN()){return (int)Python3Parser::ADD_ASSIGN;}
   if (ctx->SUB_ASSIGN()){return (int)Python3Parser::SUB_ASSIGN;}
   if (ctx->MULT_ASSIGN()){return (int)Python3Parser::MULT_ASSIGN;}
   if (ctx->DIV_ASSIGN()){return (int)Python3Parser::DIV_ASSIGN;}
   if (ctx->IDIV_ASSIGN()){return (int)Python3Parser::IDIV_ASSIGN;}
   if (ctx->MOD_ASSIGN()){return (int)Python3Parser::MOD_ASSIGN;}
  }

  virtual std::any visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) override {
    if (ctx->break_stmt()){return (int)Python3Parser::BREAK;}
    if (ctx->continue_stmt()){return (int)Python3Parser::CONTINUE;}
    return visitReturn_stmt(ctx->return_stmt());
  }

  virtual std::any visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) override {
    return constant((int)Python3Parser::BREAK);
  }

  virtual std::any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override {
    return constant((int)Python3Parser::CONTINUE);
  }

  virtual std::any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) override {
    if (ctx->testlist())
    {
      return constant((int)Python3Parser::RETURN,disarg(visitTestlist(ctx->testlist())));
    }
    return constant((int)Python3Parser::RETURN);
  }

  virtual std::any visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) override {
    if (ctx->if_stmt()){return visitIf_stmt(ctx->if_stmt());}
    if (ctx->while_stmt()){return visitWhile_stmt(ctx->while_stmt());}
    if (ctx->funcdef()){return visitFuncdef(ctx->funcdef());}
  }

  virtual std::any visitIf_stmt(Python3Parser::If_stmtContext *ctx) override {
    std::vector<Python3Parser::TestContext *>conditions=ctx->test();
    std::vector<Python3Parser::SuiteContext *>suites=ctx->suite();
    if (ctx->ELSE())
    {
      int sz=conditions.size();
      for (int i=0;i<sz;i++)
      {
        if (tobool(visitTest(conditions[i])))
        {
          return visitSuite(suites[i]);
        }
      }
      return visitSuite(suites[sz]);
    }
    else
    {
      int sz=conditions.size();
      for (int i=0;i<sz;i++)
      {
        if (tobool(visitTest(conditions[i])))
        {
          return visitSuite(suites[i]);
        }
      }
      return none();
    }
  }

  virtual std::any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override {
    while (tobool(visitTest(ctx->test())))
    {
      std::any x=visitSuite(ctx->suite());
      if (x.type()==typeid(constant))
      {
        constant y=std::any_cast<constant>(x);
        if (y.constid==(int)Python3Parser::BREAK)
        {
          break;
        }
        else if (y.constid==(int)Python3Parser::CONTINUE)
        {
          continue;
        }
        else if (y.constid==(int)Python3Parser::RETURN)
        {
          return x;
        }
      }
    }
    return none();
  }

  virtual std::any visitSuite(Python3Parser::SuiteContext *ctx) override {
    if (ctx->simple_stmt()){return visitSimple_stmt(ctx->simple_stmt());}
    else
    {
      std::vector<Python3Parser::StmtContext *>v=std::any_cast<std::vector<Python3Parser::StmtContext *> >(ctx->stmt());
      int sz=v.size();
      for (int i=0;i<sz;i++)
      {
        std::any x=visitStmt(v[i]);
        if (x.type()==typeid(constant))
        {
          return x;
        }
      }
      return none();
    }
  }

  virtual std::any visitTest(Python3Parser::TestContext *ctx) override {
    return visitOr_test(ctx->or_test());
  }

  virtual std::any visitOr_test(Python3Parser::Or_testContext *ctx) override {
    std::vector<Python3Parser::And_testContext*>v=ctx->and_test();
    bool res=0;int sz=v.size();
    if (sz==1){return visitAnd_test(v[0]);}
    for (int i=0;i<sz;i++)
    {
      res|=tobool(disarg(visitAnd_test(v[i])));
      if (res){break;}
    }
    return res;
  }

  virtual std::any visitAnd_test(Python3Parser::And_testContext *ctx) override {
    std::vector<Python3Parser::Not_testContext*>v=ctx->not_test();
    bool res=1;int sz=v.size();
    if (sz==1){return visitNot_test(v[0]);}
    for (int i=0;i<sz;i++)
    {
      res&=tobool(disarg(visitNot_test(v[i])));
      if (res==0){break;}
    }
    return res;
  }

  virtual std::any visitNot_test(Python3Parser::Not_testContext *ctx) override {
    if (ctx->not_test()){return !tobool(disarg(visitNot_test(ctx->not_test())));}
    return visitComparison(ctx->comparison()); 
  }

  virtual std::any visitComparison(Python3Parser::ComparisonContext *ctx) override {
    std::vector<Python3Parser::Arith_exprContext*>varith_expr=ctx->arith_expr();
    std::vector<Python3Parser::Comp_opContext*>vcomp_op=ctx->comp_op();
    bool res=1;int sz=vcomp_op.size();
    if (sz==0){return visitArith_expr(varith_expr[0]);}
    std::any lstval=disarg(visitArith_expr(varith_expr[0]));
    for (int i=0;i<sz;i++)
    {
      int comp=std::any_cast<int>(visitComp_op(vcomp_op[i]));bool res2=0;
      std::any curval=disarg(visitArith_expr(varith_expr[i+1]));
      if (lstval.type()!=typeid(std::string)&&curval.type()!=typeid(std::string))
      {
        if (lstval.type()!=typeid(none)&&curval.type()!=typeid(none))
        {
          if (lstval.type()!=typeid(double)&&curval.type()!=typeid(double))
        {
          int2048 lst=toint2048(lstval),cur=toint2048(curval);
          if (comp==(int)Python3Parser::LESS_THAN)
          {
            res2=(lst<cur);
          }
          else if (comp==(int)Python3Parser::GREATER_THAN)
          {
            res2=(lst>cur);
          }
          else if (comp==(int)Python3Parser::EQUALS)
          {
            res2=(lst==cur);
          }
          else if (comp==(int)Python3Parser::GT_EQ)
          {
            res2=(lst>=cur);
          }
          else if (comp==(int)Python3Parser::LT_EQ)
          {
            res2=(lst<=cur);
          }
          else if (comp==(int)Python3Parser::NOT_EQ_2)
          {
            res2=(lst!=cur);
          }
        }
        else
        {
          double lst=todouble(lstval),cur=todouble(curval);
          if (comp==(int)Python3Parser::LESS_THAN)
          {
            res2=(lst<cur);
          }
          else if (comp==(int)Python3Parser::GREATER_THAN)
          {
            res2=(lst>cur);
          }
          else if (comp==(int)Python3Parser::EQUALS)
          {
            res2=(lst==cur);
          }
          else if (comp==(int)Python3Parser::GT_EQ)
          {
            res2=(lst>=cur);
          }
          else if (comp==(int)Python3Parser::LT_EQ)
          {
            res2=(lst<=cur);
          }
          else if (comp==(int)Python3Parser::NOT_EQ_2)
          {
            res2=(lst!=cur);
          }
        }
        }
        else if (lstval.type()==typeid(none)&&curval.type()==typeid(none))
        {
          if (comp==(int)Python3Parser::EQUALS)
          {
            res2=1;
          }
          else
          {
            res2=0;
          }
        }
        else
        {
          if (comp==(int)Python3Parser::EQUALS)
          {
            res2=0;
          }
          else
          {
            res2=1;
          }
        }
      }
      else if (lstval.type()==typeid(std::string)&&curval.type()==typeid(std::string))
      {
        std::string lst=std::any_cast<std::string>(lstval),cur=std::any_cast<std::string>(curval);
        if (comp==(int)Python3Parser::LESS_THAN)
        {
          res2=(lst<cur);
        }
        else if (comp==(int)Python3Parser::GREATER_THAN)
        {
          res2=(lst>cur);
        }
        else if (comp==(int)Python3Parser::EQUALS)
        {
          res2=(lst==cur);
        }
        else if (comp==(int)Python3Parser::GT_EQ)
        {
          res2=(lst>=cur);
        }
        else if (comp==(int)Python3Parser::LT_EQ)
        {
          res2=(lst<=cur);
        }
        else if (comp==(int)Python3Parser::NOT_EQ_2)
        {
          res2=(lst!=cur);
        }
      }
      else
      {
        res2=0;//非string与string比较返回False
      }
      res&=res2;
      if (res==0)
      {break;}
      lstval=curval;
    }
    return res;
  }

  virtual std::any visitComp_op(Python3Parser::Comp_opContext *ctx) override {
   if (ctx->LESS_THAN()){return(int)Python3Parser::LESS_THAN;}
   if (ctx->GREATER_THAN()){return (int)Python3Parser::GREATER_THAN;}
   if (ctx->EQUALS()){return (int)Python3Parser::EQUALS;}
   if (ctx->GT_EQ()){return (int)Python3Parser::GT_EQ;}
   if (ctx->LT_EQ()){return (int)Python3Parser::LT_EQ;}
   if (ctx->NOT_EQ_2()){return (int)Python3Parser::NOT_EQ_2;}

  }

  virtual std::any visitArith_expr(Python3Parser::Arith_exprContext *ctx) override {
    std::vector<Python3Parser::TermContext*>vterm=ctx->term();
    std::vector<Python3Parser::Addorsub_opContext*>vop=ctx->addorsub_op();
    int sz=vterm.size();
    if (sz==1){return visitTerm(vterm[0]);}
    std::any res=disarg(visitTerm(vterm[0]));
    for (int i=1;i<sz;i++)
    {
      int op=std::any_cast<int>(visitAddorsub_op(vop[i-1]));
      std::any val=disarg(visitTerm(vterm[i]));
      if (op==(int)Python3Parser::ADD)
      {
          if (res.type()==typeid(std::string))
          {
            res=std::any_cast<std::string>(res)+std::any_cast<std::string>(val);
          }
          else if (res.type()==typeid(double)||val.type()==typeid(double))
          {
            res=todouble(res)+todouble(val);
          }
          else
          {
            res=toint2048(res)+toint2048(val);
          }
      }
      else if (op==(int)Python3Parser::MINUS)
      {
          if (res.type()==typeid(double)||val.type()==typeid(double))
          {
            res=todouble(res)-todouble(val);
          }
          else
          {
            res=toint2048(res)-toint2048(val);
          }
      }
    }
    return res;
  }

  virtual std::any visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx) override {
   if (ctx->ADD()){return (int)Python3Parser::ADD;}
   if (ctx->MINUS()){return (int)Python3Parser::MINUS;}
  }

  virtual std::any visitTerm(Python3Parser::TermContext *ctx) override {
    std::vector<Python3Parser::FactorContext*>vfactor=ctx->factor();
    std::vector<Python3Parser::Muldivmod_opContext*>vop=ctx->muldivmod_op();
    int sz=vfactor.size();
    if (sz==1){return visitFactor(vfactor[0]);}
    std::any res=disarg(visitFactor(vfactor[0]));
    for (int i=1;i<sz;i++)
    {
      int op=std::any_cast<int>(visitMuldivmod_op(vop[i-1]));
      std::any val=disarg(visitFactor(vfactor[i]));
      if (op==(int)Python3Parser::STAR)
      {
          if (res.type()==typeid(std::string))
          {
            std::string s1="",s2=std::any_cast<std::string>(res);
            int2048 t=toint2048(val);
            while (t>=0)
            {
              s1+=s2;
              t-=1;
            }
            res=s1;
          }
          if (res.type()==typeid(double)||val.type()==typeid(double))
          {
            res=todouble(res)*todouble(val);
          }
          else
          {
            res=toint2048(res)*toint2048(val);
          }
      }
      else if (op==(int)Python3Parser::IDIV)
      {
          if (res.type()==typeid(double)||val.type()==typeid(double))
          {
            res=int2048(todouble(res)/todouble(val));
          }
          else
          {
            res=toint2048(res)/toint2048(val);
          }
      }
      else if (op==(int)Python3Parser::DIV)
      {
          res=todouble(res)/todouble(val);
      }
      else if (op==(int)Python3Parser::MOD)
      {
          if (res.type()==typeid(double)||val.type()==typeid(double))
          {
            double r=todouble(res),v=todouble(val);
            int k=int(r/v);
            res=r-v*k;
          }
          else
          {
            res=toint2048(res)%toint2048(val);
          }
      }
    }
    return res;
  }

  virtual std::any visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx) override {
   if (ctx->STAR()){return (int)Python3Parser::STAR;}
   if (ctx->DIV()){return (int)Python3Parser::DIV;}
   if (ctx->IDIV()){return (int)Python3Parser::IDIV;}
   if (ctx->MOD()){return(int)Python3Parser::MOD;}
  }

  virtual std::any visitFactor(Python3Parser::FactorContext *ctx) override {
    if (ctx->factor())
    {
      std::any res=disarg(visitFactor(ctx->factor()));
      if (ctx->MINUS())
      {
        if (res.type()==typeid(double))
        {
          res=-std::any_cast<double>(res);
        }
        else
        {
          res=-toint2048(res);
        }
      }
      return res;
    }
    else
    {
      return visitAtom_expr(ctx->atom_expr());
    }
  }

  virtual std::any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override {
    if (ctx->trailer())
    {
      std::string funcname=ctx->atom()->getText();
      std::vector<arg>arglist=std::any_cast<std::vector<arg> >(visitTrailer(ctx->trailer()));
      return funcCalc(funcname,arglist);
    }
    else
    {
      return visitAtom(ctx->atom());
    }
  }

  virtual std::any visitTrailer(Python3Parser::TrailerContext *ctx) override {
    if (ctx->arglist())
    {return visitArglist(ctx->arglist());}
    else
    {
      std::vector<arg>v;
      return v;
    }
  }

  virtual std::any visitAtom(Python3Parser::AtomContext *ctx) override {
    if (ctx->NAME())
    {
      std::string argname=ctx->NAME()->getText();
      if (!scope.varid.count(argname))
      {
        scope.varRegister(argname,none());
      }
      return arg(argname,scope.varGet(argname));
    }
    else if (ctx->NUMBER())
    {
      std::string number=ctx->NUMBER()->getText();
      int len=number.length();
      if (has(number,'b')||has(number,'B'))
      {
        int2048 res=0;
        for (int i=0;i<len;i++)
        {
          if (number[i]=='0'){res=res+res;}
          else if (number[i]=='1'){res=res+res+1;}
        }
        return res;
      }
      else if (has(number,'o')||has(number,'O'))
      {
        int2048 res=0;
        for (int i=0;i<len;i++)
        {
          if (number[i]>='0'&&number[i]<='7')
          {
            res=res*8+(number[i]-'0');
          }
        }
        return res;
      }
      else if (has(number,'x')||has(number,'X'))
      {
        int2048 res=0;
        for (int i=0;i<len;i++)
        {
          if (number[i]>='0'&&number[i]<='9')
          {
            res=res*16+(number[i]-'0');
          }
          else if (number[i]>='a'&&number[i]<='f')
          {
             res=res*16+(number[i]-'a'+10);
          }
          else if (number[i]>='A'&&number[i]<='F')
          {
            res=res*16+(number[i]-'A'+10);
          }
        }
        return res;
      }
      else if (has(number,'e')||has(number,'E'))
      {
        int id=(has(number,'e')?number.find('e'):number.find('E'));
        double res=todouble(number.substr(0,id-1));
        int pw=toint(number.substr(id+1,number.length()-1));
        while (pw<0){res/=10;pw++;}
        while (pw>0){res*=10;pw--;}
        return res;
      }
      else if (has(number,'.'))
      {
        return todouble(number);
      }
      else
      {
        return int2048(number);//记得之后改为toint2048!!!
      }
    }
    else if (ctx->NONE())
    {
      return none();
    }
    else if (ctx->TRUE())
    {
      return true;
    }
    else if (ctx->FALSE())
    {
      return false;
    }
    else if (ctx->test())
    {
      return visitTest(ctx->test());
    }
    else if (ctx->format_string())
    {
      return visitFormat_string(ctx->format_string());
    }
    else//ctx->String()
    {
      std::vector<antlr4::tree::TerminalNode *>strings=ctx->STRING();
      int sz=strings.size();std::string res="";
      for (int i=0;i<sz;i++)
      {
        std::string temp=strings[i]->getText();
        res+=temp.substr(1,temp.length()-2);
      }
      return res;
    }
  }

  virtual std::any visitFormat_string(Python3Parser::Format_stringContext *ctx) override {
    /*std::vector<antlr4::tree::TerminalNode *>vstr=ctx->FORMAT_STRING_LITERAL();
    int len1=vstr.size(),len2=vtest.size(),p1=0,p2=0;*/
    std::vector<Python3Parser::TestlistContext *>vtest=ctx->testlist();
    std::string res="";
    std::string s=ctx->getText();int len=s.length();
    std::string temp="";
    int cnt=0,flg=0;
    for (int i=2;i<len-1;i++)
    {
      if (!flg)
      {
        if (s[i]=='{')
        {
          if (s[i+1]=='{')
          {
            temp+='{';i++;
          }
          else
          {
            res+=temp;temp="";flg=1;res+=tostring(disarg(visitTestlist(vtest[cnt++])));
          }
        }
        else if (s[i]=='}')
        {
          temp+='}';i++;
        }
        else
        {
          temp+=s[i];
        }
      }
      else if (s[i]=='}')
      {
        flg=0;
      }
    }
    res+=temp;
    /*while (p1<len1||p2<len2)
    {
      if (p1==len1){res+=tostring(disarg(visitTestlist(vtest[p2++])));}
      else if (p2==len2){res+=vstr[p1++]->getText();}
      else if ((vstr[p1]->getCharPositionInLine())<(ctx->OPEN_BRACE(p2)->getCharPositionInLine()))
      {
        res+=vstr[p1++]->getText();
      }
      else
      {
        res+=tostring(disarg(visitTestlist(vtest[p2++])));
      }
    }*/

    return res;
  }

  virtual std::any visitTestlist(Python3Parser::TestlistContext *ctx) override {
   std::vector<Python3Parser::TestContext *>vtest=ctx->test();
   int sz=vtest.size();
   if (sz==1){return visitTest(vtest[0]);}
   std::vector<std::any>res;
   for (int i=0;i<sz;i++)
   {
    res.push_back(visitTest(vtest[i]));
   }
   return res;
  }

  virtual std::any visitArglist(Python3Parser::ArglistContext *ctx) override {
    std::vector<Python3Parser::ArgumentContext *>argumentlist=ctx->argument();
    std::vector<arg>arglist;
    int sz=argumentlist.size();
    for (int i=0;i<sz;i++)
    {
      arglist.push_back(std::any_cast<arg>(visitArgument(argumentlist[i])));
    }
    return arglist;
  }

  virtual std::any visitArgument(Python3Parser::ArgumentContext *ctx) override {
    if (ctx->ASSIGN())
    {
      std::string argname=std::any_cast<arg>(visitTest(ctx->test(0))).argname;
      std::any init_val=disarg(visitTest(ctx->test(1)));
      scope.varRegister(argname,init_val);
      return arg(argname,init_val);
    }
    else
    {
      std::string argname="";
      std::any init_val=disarg(visitTest(ctx->test(0)));
      return arg(argname,init_val);
    }
  }
};


#endif//PYTHON_INTERPRETER_EVALVISITOR_H
