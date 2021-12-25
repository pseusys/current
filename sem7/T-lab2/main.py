from argparse import ArgumentParser

from pages import MainPage


parser = ArgumentParser(description="Run test cases for github.com, test cases described in './README.md'.")
parser.add_argument("username", help="Your username for github.com")
parser.add_argument("password", help="Your password for github.com")
parser.add_argument("repo name", help="Name of repository to create")
parser.add_argument("-d", "--repo-description", default="", help="Description of repository to create")
parser.add_argument("-p", "--private", type=bool, default=False, help="True to make repository private, False - public")
parser.add_argument("-r", "--readme.md", type=bool, default=False, help="True to generate README.md file, False to not")

args = vars(parser.parse_args())
print("Testing suite 1: signing in")
main = MainPage().login().login(args["username"], args["password"])
print(f"User repositories number: {main.repo_number()}")
if main.has_repo(args["repo name"]):
    print("Can not test suite 2, repo with given name already exists!")
    print("Opening the existing repo page to test suite 3")
    repo = main.open_repo(args["repo name"])
else:
    print("Testing suite 2: creating repo")
    repo = main.new_repo().create_repo(args["repo name"], args["repo_description"], args["private"], args["readme.md"])
    print("Repository testing results:")
    if args['readme.md']:
        print(f"    Description: '{repo.get_description()}', {'successful' if repo.get_description() == args['repo_description'] else 'failed'}")
    print(f"    Private: '{repo.is_private()}', {'successful' if repo.is_private() == args['private'] else 'failed'}")
    print(f"    Readme.md: '{repo.has_readme()}', {'successful' if repo.has_readme() == args['readme.md'] else 'failed'}")
print("Testing suite 3: deleting repo")
repo.open_settings().delete_repo()
print(f"Repository link check {'failed' if main.has_repo(args['repo name']) else 'successful'}, repositories number: {main.repo_number()}")
print("Testing suite 3: logging out")
main.logout()
print("Testing successful!")
