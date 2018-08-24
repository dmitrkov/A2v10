﻿// Copyright © 2015-2017 Alex Kukhtin. All rights reserved.

using System;
using System.Web;

using A2v10.Data;
using A2v10.Data.Interfaces;
using A2v10.Infrastructure;
using A2v10.Messaging;
using A2v10.Request;
using A2v10.Web.Config;
using A2v10.Web.Identity;
using A2v10.Workflow;
using A2v10.Xaml;

namespace A2v10.Web.Mvc.Start
{
	public partial class Startup
	{
		public void StartServices()
		{
			// DI ready
			ServiceLocator.Start = (IServiceLocator locator) =>
			{
				IProfiler profiler = new WebProfiler();
				IApplicationHost host = new WebApplicationHost(profiler);
				ILocalizer localizer = new WebLocalizer(host);
				IDbContext dbContext = new SqlDbContext(
					profiler as IDataProfiler,
					host as IDataConfiguration,
					localizer as IDataLocalizer,
					host as ITenantManager);
				IRenderer renderer = new XamlRenderer(profiler);
				IWorkflowEngine workflowEngine = new WorkflowEngine(host, dbContext);
				IMessaging messaging = new MessageProcessor(host, dbContext);
				IDataScripter scripter = new VueDataScripter();
				ILogger logger = new WebLogger(host, dbContext);
				IMessageService emailService = new EmailService(logger);
				ISmsService smsService = new SmsService(dbContext, logger);

				locator.RegisterService<IDbContext>(dbContext);
				locator.RegisterService<IProfiler>(profiler);
				locator.RegisterService<IApplicationHost>(host);
				locator.RegisterService<IRenderer>(renderer);
				locator.RegisterService<IWorkflowEngine>(workflowEngine);
				locator.RegisterService<IMessaging>(messaging);
				locator.RegisterService<ILocalizer>(localizer);
				locator.RegisterService<IDataScripter>(scripter);
				locator.RegisterService<ILogger>(logger);
				locator.RegisterService<IMessageService>(emailService);
				locator.RegisterService<ISmsService>(smsService);

				HttpContext.Current.Items.Add("ServiceLocator", locator);
			};

			ServiceLocator.GetCurrentLocator = () =>
			{
				if (HttpContext.Current == null)
					throw new InvalidProgramException("There is no http context");
				var locator = HttpContext.Current.Items["ServiceLocator"];
				if (locator == null)
					new ServiceLocator();
				return HttpContext.Current.Items["ServiceLocator"] as IServiceLocator;
			};
		}
	}
}
