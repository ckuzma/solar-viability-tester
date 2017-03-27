# cf api https://api.ng.bluemix.net
# cf login -u <email_login> -o <email_login>
cf target -s dev
cf push solar-viability-tester
# cf logs solar-viability-tester
