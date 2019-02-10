﻿
using System;
using System.Collections.Generic;
using System.Dynamic;
using System.IO;
using System.Web;

using A2v10.Infrastructure;
using A2v10.Request;

namespace A2v10.Runtime
{
	public class DesktopRequest
	{
		BaseController _controller = new BaseController();

		public String MimeType { get; private set; }

		const String MIME_JSON   = "application/json";
		const String MIME_HTML   = "text/html";
		const String MIME_SCRIPT = "application/javascript";
		const String MIME_STYLE  = "text/css";

		public String ProcessRequest(String url, String search, String postData)
		{
			using (_controller.Host.Profiler.BeginRequest(url, null) as IDisposable)
			{
				try
				{
					return ProcessRequestImpl(url, search, postData);
				}
				catch (Exception /*err*/)
				{
					throw;
				}
			}
		}

		String ProcessRequestImpl(String url, String search, String postData)
		{
			if (url.StartsWith("admin/"))
			{
				url = url.Substring(6);
				_controller.Admin = true;
			}
			try
			{
				MimeType = MIME_HTML;
				using (var writer = new StringWriter())
				{
					if (String.IsNullOrEmpty(url))
						RenderIndex(writer);
					else if (url.StartsWith("shell/"))
					{
						Shell(url.Substring(6).ToLowerInvariant(), writer, out String shellMime);
						MimeType = shellMime;
					}
					else if (url.StartsWith("_page/"))
						Render(RequestUrlKind.Page, url.Substring(6), search, writer);
					else if (url.StartsWith("_dialog/"))
						Render(RequestUrlKind.Dialog, url.Substring(8), search, writer);
					else if (url.StartsWith("_popup/"))
						Render(RequestUrlKind.Popup, url.Substring(7), search, writer);
					else if (url.StartsWith("_data/"))
					{
						var command = url.Substring(6);
						var rb = new DesktopResponse(writer);
						_controller.Data(command, SetSqlParams, postData, rb).Wait();
						MimeType = rb.ContentType;
					}
					else if (url.StartsWith("_image/"))
					{
						_controller.DownloadAttachment("/" + url, SetSqlParams).Wait(); // with _image prefix
					}
					else
						RenderIndex(writer);
					return writer.ToString();
				}
			}
			catch (Exception ex)
			{
				if (ex.InnerException != null)
					ex = ex.InnerException;
				// TODO:: /exception
				return $"<div>{ex.Message}</div>";
			}
		}

		public void SetSqlParams(ExpandoObject prms)
		{
			// TODO: current user ID;
			A2v10.Infrastructure.DynamicHelpers.Set(prms, "UserId", 50);
			A2v10.Infrastructure.DynamicHelpers.Set(prms, "TenantId", 1);
		}

		void Render(RequestUrlKind kind, String path, String search, TextWriter writer)
		{
			ExpandoObject loadPrms = new ExpandoObject();
			loadPrms.Append(HttpUtility.ParseQueryString(search), toPascalCase: true);
			SetSqlParams(loadPrms);
			_controller.RenderElementKind(kind, path, loadPrms, writer).Wait();
		}

		void RenderIndex(TextWriter writer)
		{
			// TODO: userName
			String userName = "User Name";
			String locale = "uk"; // TODO: ctrl.CurrentLang
			String theme = "site"; // TODO: ctrl.Host.Theme
			var prms = new Dictionary<String, String>
				{
					{ "$(RootUrl)", String.Empty },
					{ "$(HelpUrl)", _controller.Host.HelpUrl },
					{ "$(PersonName)", userName },
					{ "$(Theme)", theme },
					{ "$(Build)", _controller.Host.AppBuild },
					{ "$(Locale)", locale },
					{ "$(Minify)", _controller.IsDebugConfiguration ? String.Empty : "min." },
					{ "$(Description)", _controller.Host.AppDescription }
				};
			_controller.Layout(writer, prms);
		}

		public void Shell(String url, TextWriter writer, out String mimeType)
		{
			mimeType = MIME_SCRIPT;
			switch (url)
			{
				case "appstyles":
					_controller.GetAppStyleConent(writer);
					mimeType = MIME_STYLE;
					break;
				case "appscripts":
					_controller.GetAppScriptConent(writer);
					mimeType = MIME_STYLE;
					break;
				case "script":
					try
					{
						_controller.ShellScript(null, SetSqlParams, userAdmin: true, bAdmin: false, writer: writer).Wait();
						mimeType = MIME_SCRIPT;
					}
					catch (Exception ex)
					{
						if (ex.InnerException != null)
							ex = ex.InnerException;
						writer.Write($"alert('{ex.Message.EncodeJs()}')");
					}
					break;
				case "trace":
					String json = _controller.Host.Profiler.GetJson();
					mimeType = MIME_JSON;
					writer.Write(json);
					break;
			}
		}
	}
}