﻿#pragma warning disable 1591
//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:4.0.30319.42000
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace ASP
{
    using System;
    using System.Collections.Generic;
    using System.Dynamic;
    using System.IO;
    using System.Linq;
    using System.Net;
    using System.Text;
    using System.Web;
    using System.Web.Helpers;
    using System.Web.Mvc;
    using System.Web.Mvc.Ajax;
    using System.Web.Mvc.Html;
    using System.Web.Routing;
    using System.Web.Security;
    using System.Web.UI;
    using System.Web.WebPages;
    using A2v10.Web.Mvc;
    using Microsoft.AspNet.Identity;
    
    [System.CodeDom.Compiler.GeneratedCodeAttribute("RazorGenerator", "2.0.0.0")]
    [System.Web.WebPages.PageVirtualPathAttribute("~/Views/Account/ResetPasswordConfirmation.cshtml")]
    public partial class _Views_Account_ResetPasswordConfirmation_cshtml : System.Web.Mvc.WebViewPage<dynamic>
    {
        public _Views_Account_ResetPasswordConfirmation_cshtml()
        {
        }
        public override void Execute()
        {
            
            #line 1 "..\..\Views\Account\ResetPasswordConfirmation.cshtml"
  
    ViewBag.Title = "Reset password confirmation";

            
            #line default
            #line hidden
WriteLiteral("\r\n\r\n<hgroup");

WriteLiteral(" class=\"title\"");

WriteLiteral(">\r\n    <h1>");

            
            #line 6 "..\..\Views\Account\ResetPasswordConfirmation.cshtml"
   Write(ViewBag.Title);

            
            #line default
            #line hidden
WriteLiteral(".</h1>\r\n</hgroup>\r\n<div>\r\n    <p>\r\n        Your password has been reset. Please ");

            
            #line 10 "..\..\Views\Account\ResetPasswordConfirmation.cshtml"
                                        Write(Html.ActionLink("click here to log in", "Login", "Account", routeValues: null, htmlAttributes: new { id = "loginLink" }));

            
            #line default
            #line hidden
WriteLiteral("\r\n    </p>\r\n</div>\r\n");

        }
    }
}
#pragma warning restore 1591
