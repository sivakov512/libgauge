.PHONY: install lint format test check

install:
	uv sync

lint:
	uv run ruff check .
	uv run ruff format --check .
	uv run basedpyright
	uv run mypy gauge tests

format:
	uv run ruff check --fix .
	uv run ruff format .

test:
	uv run pytest

check: lint test
